# overview
This repository was created to illustrate the following PR.
- https://github.com/ros2/rclcpp/pull/2091
- https://github.com/ros2/ros2_tracing/pull/30

## contents

### application
The cyclic_pipeline demo included in this repository differs from [the officially distributed one](https://github.com/ros2/demos/tree/rolling/intra_process_demo/src/cyclic_pipeline) in that each node has two subscriptions. One node publishes as in the official demo. The remaining node receives messages but does not publish. The official demo was implemented to run on a single thread, so we have made changes here as well.

### tracedata
The trace data, including trace points added by PR, is included in tracedata_sample.
- 1pub-2sub-multithread [branch:multi-thread]
  - Trace data from the cyclic_pipeline demo using MultiThreadedExecutor and multiple threads.

- 1pub-2sub-two-thread [branch:master]
  - Trace data from the cyclic_pipeline demo using SingleThreadedExecutor and explicitly using two threads.