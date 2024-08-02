#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Drive the robot with velocities info");

    //Request velocities info
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;
    
    //Call the motor_command service
    if (!client.call(srv))
        ROS_ERROR("Failed to call service motor_command");
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    bool is_ball_visible = false;
    int left_bound = img.width/3;
    int right_bound = 2*left_bound;



    for (int i = 0; i <= img.height*img.step; i += 3){
	int R = img.data[i]; //Red
	int G = img.data[i]; //Green
	int B = img.data[i]; //Blue

        if (R==white_pixel && G==white_pixel && B==white_pixel){
		is_ball_visible = true;
		int pixel_pos = i % img.step/3;

		if (pixel_pos < left_bound){
			drive_bot(0.5, 0.5);
		}
		else if (pixel_pos < right_bound){
			drive_bot(0.5, 0.0);
		}
		else {
			drive_bot(0.5, -0.5);
		}
		break;
            }
	else {
		drive_bot(0.0, 0.0);
		}
    }
    if (!is_ball_visible) {
	drive_bot(0.0, 0.0);
	}

}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
