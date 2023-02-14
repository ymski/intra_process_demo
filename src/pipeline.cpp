#include <chrono>
#include <cinttypes>
#include <cstdio>
#include <memory>
#include <string>
#include <utility>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/int32.hpp"
using std::placeholders::_1;
using namespace std::chrono_literals;

class IncrementerPipe : public rclcpp::Node
{
  public:
	IncrementerPipe(const std::string & name, const std::string & in, const std::string & out)
  : Node(name, rclcpp::NodeOptions().use_intra_process_comms(true))
	{
    pub = this->create_publisher<std_msgs::msg::Int32>(out, 10);
		sub1 = this->create_subscription<std_msgs::msg::Int32>(
      in,
      10,
      std::bind(&IncrementerPipe::subscription_callback, this, _1));
    sub2 = this->create_subscription<std_msgs::msg::Int32>(
      in,
      10,
      std::bind(&IncrementerPipe::empty_callback, this, _1));
	}
  rclcpp::Publisher<std_msgs::msg::Int32>::SharedPtr pub;

  private:
  void subscription_callback(std_msgs::msg::Int32::UniquePtr msg)
  {
    auto pub_ptr = this->pub;
    if (!pub_ptr) {
      return;
    }
    printf(
      "Sub1 received message with value:         %d, and address: 0x%" PRIXPTR "\n", msg->data,
      reinterpret_cast<std::uintptr_t>(msg.get()));
    printf("  sleeping for 1 second...\n");
    if (!rclcpp::sleep_for(1s)) {
      return;    // Return if the sleep failed (e.g. on ctrl-c).
    }
    printf("  done.\n");
    msg->data++;    // Increment the message's data.
    printf(
      "Incrementing and sending with value: %d, and address: 0x%" PRIXPTR "\n", msg->data,
      reinterpret_cast<std::uintptr_t>(msg.get()));
    pub_ptr->publish(std::move(msg));    // Send the message along to the output topic.
  };

  void empty_callback(std_msgs::msg::Int32::UniquePtr msg)
  {
    printf(
      "SUB2 received message with value:         %d, and address: 0x%" PRIXPTR "\n", msg->data,
      reinterpret_cast<std::uintptr_t>(msg.get()));
    printf("  sleeping for 1 second...\n");
    if (!rclcpp::sleep_for(1s)) {
      return;    // Return if the sleep failed (e.g. on ctrl-c).
    }
    printf("  done.\n");

  };

  // sub1 does not publih
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub1;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr sub2;
};

int main(int argc, char * argv[])
{
  setvbuf(stdout, NULL, _IONBF, BUFSIZ);
  rclcpp::init(argc, argv);
  rclcpp::executors::SingleThreadedExecutor executor1;
  rclcpp::executors::SingleThreadedExecutor executor2;

  // Create a simple loop by connecting the in and out topics of two IncrementerPipe's.
  // The expectation is that the address of the message being passed between them never changes.
  auto pipe1 = std::make_shared<IncrementerPipe>("pipe1", "topic1", "topic2");
  auto pipe2 = std::make_shared<IncrementerPipe>("pipe2", "topic2", "topic1");
  rclcpp::sleep_for(1s);  // Wait for subscriptions to be established to avoid race conditions.
  // Publish the first message (kicking off the cycle).
  std::unique_ptr<std_msgs::msg::Int32> msg(new std_msgs::msg::Int32());
  msg->data = 1;
  printf(
    "Published first message with value:  %d, and address: 0x%" PRIXPTR "\n", msg->data,
    reinterpret_cast<std::uintptr_t>(msg.get()));
  pipe1->pub->publish(std::move(msg));


  executor1.add_node(pipe1);
  executor1.add_node(pipe2);
  executor1.spin();
  // executor2.add_node(pipe2);
  // std::thread th1(spin_void(executor1), 1);
  // std::thread th2(spin_void(executor2), 1);
  // th1.join();
  // th2.join();

  rclcpp::shutdown();

  return 0;
}