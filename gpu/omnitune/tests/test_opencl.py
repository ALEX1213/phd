from unittest import main

from omnitune import opencl
from phd.lib.labm8.tests.testutil import TestCase


class TestOpenCL(TestCase):

  # get_devices()
  def test_get_devices(self):
    self._test(True,
               isinstance(opencl.get_devices(), list))


if __name__ == '__main__':
  main()
