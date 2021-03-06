// A binary that reads an AddXandY message from stdin, and writes an AddXandY
// message to stdout.
//
// If the input AddXandY.x == 10, the program crashes.

#include "labm8/data/test/ppar/protos.pb.h"
#include "phd/macros.h"
#include "phd/pbutil.h"

void ProcessProtobuf(const AddXandY& input_proto,
                     AddXandY* output_proto) {
  int x = input_proto.x();
  int y = input_proto.y();

  CHECK(x != 10);

  INFO("Adding %d and %d and storing the result in a new message", x, y);
  output_proto->set_result(x + y);
}

PBUTIL_PROCESS_MAIN(ProcessProtobuf, AddXandY, AddXandY);
