"""Tests for //deeplearning/deepsmith:result."""
import datetime
import sys
import tempfile

import pytest
from absl import app

import deeplearning.deepsmith.client
import deeplearning.deepsmith.generator
import deeplearning.deepsmith.harness
import deeplearning.deepsmith.profiling_event
import deeplearning.deepsmith.result
import deeplearning.deepsmith.testcase
from deeplearning.deepsmith import datastore
from deeplearning.deepsmith import db
from deeplearning.deepsmith.proto import deepsmith_pb2


@pytest.fixture
def session() -> db.session_t:
  with tempfile.TemporaryDirectory(prefix="dsmith-test-db-") as tmpdir:
    ds = datastore.DataStore(engine="sqlite", db_dir=tmpdir)
    with ds.Session() as session_:
      yield session_


def test_Result_ToProto():
  now = datetime.datetime.now()

  result = deeplearning.deepsmith.result.Result(
      testcase=deeplearning.deepsmith.testcase.Testcase(
          toolchain=deeplearning.deepsmith.toolchain.Toolchain(name="cpp"),
          generator=deeplearning.deepsmith.generator.Generator(name="generator"),
          harness=deeplearning.deepsmith.harness.Harness(name="harness"),
          inputset=[
            deeplearning.deepsmith.testcase.TestcaseInput(
                name=deeplearning.deepsmith.testcase.TestcaseInputName(name="src"),
                value=deeplearning.deepsmith.testcase.TestcaseInputValue(string="void main() {}"),
            ),
            deeplearning.deepsmith.testcase.TestcaseInput(
                name=deeplearning.deepsmith.testcase.TestcaseInputName(name="data"),
                value=deeplearning.deepsmith.testcase.TestcaseInputValue(string="[1,2]"),
            ),
          ],
          invariant_optset=[
            deeplearning.deepsmith.testcase.TestcaseInvariantOpt(
                name=deeplearning.deepsmith.testcase.TestcaseInvariantOptName(name="config"),
                value=deeplearning.deepsmith.testcase.TestcaseInvariantOptValue(name="opt"),
            ),
          ],
          profiling_events=[
            deeplearning.deepsmith.profiling_event.TestcaseProfilingEvent(
                client=deeplearning.deepsmith.client.Client(name="localhost"),
                type=deeplearning.deepsmith.profiling_event.ProfilingEventType(
                    name="generate",
                ),
                duration_seconds=1.0,
                date=now,
            ),
            deeplearning.deepsmith.profiling_event.TestcaseProfilingEvent(
                client=deeplearning.deepsmith.client.Client(name="localhost"),
                type=deeplearning.deepsmith.profiling_event.ProfilingEventType(
                    name="foo",
                ),
                duration_seconds=1.0,
                date=now,
            ),
          ]
      ),
      testbed=deeplearning.deepsmith.testbed.Testbed(
          toolchain=deeplearning.deepsmith.toolchain.Toolchain(name="cpp"),
          name="clang",
          optset=[
            deeplearning.deepsmith.testbed.TestbedOpt(
                name=deeplearning.deepsmith.testbed.TestbedOptName(name="arch"),
                value=deeplearning.deepsmith.testbed.TestbedOptValue(name="x86_64"),
            ),
            deeplearning.deepsmith.testbed.TestbedOpt(
                name=deeplearning.deepsmith.testbed.TestbedOptName(name="build"),
                value=deeplearning.deepsmith.testbed.TestbedOptValue(name="debug+assert"),
            ),
          ],
      ),
      returncode=0,
      outputset=[
        deeplearning.deepsmith.result.ResultOutput(
            name=deeplearning.deepsmith.result.ResultOutputName(name="stdout"),
            value=deeplearning.deepsmith.result.ResultOutputValue(
                truncated_value="Hello, world!"
            ),
        ),
        deeplearning.deepsmith.result.ResultOutput(
            name=deeplearning.deepsmith.result.ResultOutputName(name="stderr"),
            value=deeplearning.deepsmith.result.ResultOutputValue(
                truncated_value=""
            ),
        ),
      ],
      profiling_events=[
        deeplearning.deepsmith.profiling_event.ResultProfilingEvent(
            client=deeplearning.deepsmith.client.Client(name="localhost"),
            type=deeplearning.deepsmith.profiling_event.ProfilingEventType(
                name="exec",
            ),
            duration_seconds=10.0,
            date=now,
        ),
        deeplearning.deepsmith.profiling_event.ResultProfilingEvent(
            client=deeplearning.deepsmith.client.Client(name="localhost"),
            type=deeplearning.deepsmith.profiling_event.ProfilingEventType(
                name="overhead",
            ),
            duration_seconds=1.0,
            date=now,
        ),
      ],
  )
  proto = result.ToProto()
  assert proto.testcase.toolchain == "cpp"
  assert proto.testcase.generator.name == "generator"
  assert proto.testcase.harness.name == "harness"
  assert len(proto.testcase.inputs) == 2
  assert proto.testcase.inputs["src"] == "void main() {}"
  assert proto.testcase.inputs["data"] == "[1,2]"
  assert len(proto.testcase.invariant_opts) == 1
  assert proto.testcase.invariant_opts["config"] == "opt"
  assert len(proto.testcase.profiling_events) == 2
  assert proto.testcase.profiling_events[0].client == "localhost"
  assert proto.testcase.profiling_events[0].type == "generate"
  assert proto.testcase.profiling_events[0].client == "localhost"
  assert proto.testbed.toolchain == "cpp"
  assert proto.testbed.name == "clang"
  assert len(proto.testbed.opts) == 2
  assert proto.testbed.opts["arch"] == "x86_64"
  assert proto.testbed.opts["build"] == "debug+assert"
  assert len(proto.outputs) == 2
  assert proto.outputs["stdout"] == "Hello, world!"
  assert proto.outputs["stderr"] == ""
  assert len(proto.testcase.profiling_events) == 2
  assert proto.profiling_events[0].client == "localhost"
  assert proto.profiling_events[0].type == "exec"
  assert proto.profiling_events[0].duration_seconds == 10.0
  assert proto.profiling_events[1].client == "localhost"
  assert proto.profiling_events[1].type == "overhead"
  assert proto.profiling_events[1].duration_seconds == 1.0


def test_Generator_GetOrAdd_ToProto_equivalence(session):
  proto_in = deepsmith_pb2.Result(
      testcase=deepsmith_pb2.Testcase(
          toolchain="cpp",
          generator=deepsmith_pb2.Generator(
              name="generator"
          ),
          harness=deepsmith_pb2.Harness(
              name="harness"
          ),
          inputs={
            "src": "void main() {}",
            "data": "[1,2]",
          },
          invariant_opts={
            "config": "opt",
          },
          profiling_events=[
            deepsmith_pb2.ProfilingEvent(
                client="localhost",
                type="generate",
                duration_seconds=1.0,
                date_epoch_seconds=1123123123,
            ),
            deepsmith_pb2.ProfilingEvent(
                client="localhost",
                type="foo",
                duration_seconds=1.0,
                date_epoch_seconds=1123123123,
            ),
          ]
      ),
      testbed=deepsmith_pb2.Testbed(
          toolchain="cpp",
          name="clang",
          opts={
            "arch": "x86_64",
            "build": "debug+assert",
          },
      ),
      returncode=0,
      outputs={
        "stdout": "Hello, world!",
        "stderr": "",
      },
      profiling_events=[
        deepsmith_pb2.ProfilingEvent(
            client="localhost",
            type="exec",
            duration_seconds=10.0,
            date_epoch_seconds=1123123123,
        ),
        deepsmith_pb2.ProfilingEvent(
            client="localhost",
            type="overhead",
            duration_seconds=1.0,
            date_epoch_seconds=1123123123,
        ),
      ],
  )
  testcase = deeplearning.deepsmith.result.Result.GetOrAdd(
      session, proto_in
  )

  # NOTE: We have to flush so that SQLAlchemy resolves all of the object IDs.
  session.flush()
  proto_out = testcase.ToProto()
  assert proto_in == proto_out
  proto_out.ClearField("outputs")
  assert proto_in != proto_out  # Sanity check.


def main(argv):  # pylint: disable=missing-docstring
  del argv
  sys.exit(pytest.main([__file__, "-v"]))


if __name__ == "__main__":
  app.run(main)
