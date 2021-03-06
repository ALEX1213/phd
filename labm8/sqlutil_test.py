"""Unit tests for //labm8:sqlutil."""

import pathlib
import sys
import tempfile
import typing

import pytest
import sqlalchemy as sql
from absl import app
from sqlalchemy.ext import declarative

from labm8 import pbutil
from labm8 import sqlutil
from labm8.proto import test_protos_pb2


@pytest.fixture(scope='function')
def tempdir() -> pathlib.Path:
  """A pytest fixture for a temporary directory."""
  with tempfile.TemporaryDirectory(prefix='phd_') as d:
    yield pathlib.Path(d)


def test_CreateEngine_sqlite_not_found(tempdir: pathlib.Path):
  """Test DatabaseNotFound for non-existent SQLite database."""
  with pytest.raises(sqlutil.DatabaseNotFound) as e_ctx:
    sqlutil.CreateEngine(f'sqlite:///{tempdir.absolute()}/db.db',
                         must_exist=True)
  assert e_ctx.value.url == f'sqlite:///{tempdir.absolute()}/db.db'
  assert str(e_ctx.value) == (f"Database not found: "
                              f"'sqlite:///{tempdir.absolute()}/db.db'")


def test_CreateEngine_sqlite_invalid_relpath():
  """Test that relative paths are disabled."""
  with pytest.raises(ValueError) as e_ctx:
    sqlutil.CreateEngine(f'sqlite:///relative.db')
  assert str(e_ctx.value) == "Relative path to SQLite database is not allowed"


def test_CreateEngine_error_if_sqlite_in_memory_must_exist():
  """Error is raised if in-memory "must exist" database requested."""
  with pytest.raises(ValueError) as e_ctx:
    sqlutil.CreateEngine(f'sqlite://', must_exist=True)
  assert str(e_ctx.value) == ("must_exist=True not valid for in-memory "
                              "SQLite database")


def test_CreateEngine_sqlite_created(tempdir: pathlib.Path):
  """Test that SQLite database is found."""
  sqlutil.CreateEngine(f'sqlite:///{tempdir}/db.db')
  assert (tempdir / 'db.db').is_file()


def test_Session_GetOrAdd():
  """Test that GetOrAdd() does not create duplicates."""
  base = declarative.declarative_base()

  class Table(base, sqlutil.TablenameFromClassNameMixin):
    """A table containing a single 'value' primary key."""
    value = sql.Column(sql.Integer, primary_key=True)

  # Create the database.
  db = sqlutil.Database(f'sqlite://', base)

  # Create an entry in the database.
  with db.Session(commit=True) as s:
    s.GetOrAdd(Table, value=42)

  # Check that the entry has been added.
  with db.Session() as s:
    assert s.query(Table).count() == 1
    assert s.query(Table).one().value == 42

  # Create another entry. Since a row already exists in the database, this
  # doesn't add anything.
  with db.Session(commit=True) as s:
    s.GetOrAdd(Table, value=42)

  # Check that the database is unchanged.
  with db.Session() as s:
    assert s.query(Table).count() == 1
    assert s.query(Table).one().value == 42


class AbstractTestMessage(sqlutil.ProtoBackedMixin,
                          sqlutil.TablenameFromClassNameMixin):
  """A table containing a single 'value' primary key."""

  proto_t = test_protos_pb2.TestMessage

  string = sql.Column(sql.String, primary_key=True)
  number = sql.Column(sql.Integer)

  def SetProto(self, proto: test_protos_pb2.TestMessage) -> None:
    """Set a protocol buffer representation."""
    proto.string = self.string
    proto.number = self.number

  @staticmethod
  def FromProto(proto) -> typing.Dict[
    str, typing.Any]:
    """Instantiate an object from protocol buffer message."""
    return {
      "string": proto.string,
      "number": proto.number,
    }


def test_ProtoBackedMixin_FromProto():
  """Test FromProto constructor for proto backed tables."""
  base = declarative.declarative_base()

  class TestMessage(AbstractTestMessage, base):
    pass

  proto = test_protos_pb2.TestMessage(string="Hello, world!", number=42)
  row = TestMessage(**TestMessage.FromProto(proto))
  assert row.string == "Hello, world!"
  assert row.number == 42


def test_ProtoBackedMixin_SetProto():
  """Test SetProto method for proto backed tables."""
  base = declarative.declarative_base()

  class TestMessage(AbstractTestMessage, base):
    pass

  proto = test_protos_pb2.TestMessage()
  TestMessage(string="Hello, world!", number=42).SetProto(proto)
  assert proto.string == "Hello, world!"
  assert proto.number == 42


def test_ProtoBackedMixin_ToProto():
  """Test FromProto constructor for proto backed tables."""
  base = declarative.declarative_base()

  class TestMessage(AbstractTestMessage, base):
    pass

  row = TestMessage(string="Hello, world!", number=42)
  proto = row.ToProto()
  assert proto.string == "Hello, world!"
  assert proto.number == 42


def test_ProtoBackedMixin_FromFile(tempdir: pathlib.Path):
  """Test FromProto constructor for proto backed tables."""
  base = declarative.declarative_base()

  class TestMessage(AbstractTestMessage, base):
    pass

  pbutil.ToFile(test_protos_pb2.TestMessage(string="Hello, world!", number=42),
                tempdir / 'proto.pb')

  row = TestMessage(**TestMessage.FromFile(tempdir / 'proto.pb'))
  assert row.string == "Hello, world!"
  assert row.number == 42


def main(argv):
  if len(argv) > 1:
    raise app.UsageError('Unrecognized command line flags.')
  sys.exit(pytest.main([__file__, '-vv']))


if __name__ == '__main__':
  app.run(main)
