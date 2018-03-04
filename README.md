# Group 7 - DIT168

This is a repository for completing the assignments of the DIT168 V18 Industrial IT and Embedded Systems course

# Authors 🖋️

Måns Thörnvik, Rashad Kamsheh, Madisen Whitfield and Sarah Aldelame.

# Introduction

We -Group 7- will be using GitHub in a very professional manner, giving high priority to traceability, shared understanding, code co- ownership, maintainable version control and secure backing up. And in order to adhere to these principles, the team agrees to the following instructions & set of rules related to the following repository layout:

# Repository layout


**Branching**

when it comes to branching, we are aiming to adhere as much as we can to the following git branching model: http://nvie.com/posts/a-successful-git-branching-model/
Which can be summarized like this:

- The master branch and the develop branch will be locked, i.e. no one will be able to push code without an approved pull requests or without the requirement of the decided status checks to pass before merging.
- Each feature will have its own branch, when the feature is done, a pull request is made to merge the feature branch with the develop branch, the pull request will not be merged unless approved by at least 1 collaborator and while the pull request has no requested changes. 
- When milestones are achieved, a release branch will be created to merge the new features in the develop branch into the master branch.  
- When resolving issues or any problems that are documented in the defect backlog, a hot-fix-branch will be created to update the production code and the test code.
- Once an integration process has been completed, the develop branch will be integrated towards the master branch. The master branch, after successful integration, will have its product state incremented by one. So for a master version of 0.1, after an increment has been submitted will have version 0.2. The current version on master will be indicated using a GitHub tag.

**Peer reviews** 

All members are required to view all pull requests, but only one approval is sufficient to merge a pull request. A peer should review the pull request’s readability, functionality and coherence. e.g. (Naming conventions, document tags, comments, duplicate free code).
The peer should leave constructive and civil comments with the intention of improving the code. A pull request shall not be blocked because of very trivial things, e.g. (indentation or very minor things not relating to the definition of done), in which case the peer can simply leave a short comment on the pull request recommending the simple change. 
Peer reviews should be a way of helping the team with having a shared understanding of every part of the system. And not a way to criticize and ridicule the owner of the feature.  

# How to clone, build, and test the project

Using your terminal or git bash/interface, clone this repository using this command:

```
git clone https://github.com/maansthoernvik/Group-7---DIT168.git
```

Navigate into the cloned project.

```cd Group-7---DIT168/```

To install all dependencies that the project requires, run the ```install_dependencies``` script the lies in the root directory of the project, you might need to give permission to allow execution first.

```sudo chmod +x install_dependencies.sh```

Run the installation script.

```./install_dependencies```

After installing the project's dependencies, restart your terminal, or whichever interface you are currently using, to make sure that the changes become active. Start up another terminal and navigate back to where you left off.

Now, change directory into the examples folder.

```
cd Group-7---DIT168/examples/
```

Make the build folder and move into it.

```
mkdir build && cd build/
```

Generate a Makefile.

```
cmake ..
```

Now try the make command, you should see a "[100%] Built target" line at the end.

```
make
```

Now run make test to check if the example passes all the tests.

```
make test
``` 

Run the receiver file in the terminal in order to wait for any sent information.

```
./examples-receiver 
``` 

And for the final step, open a new terminal in the same directory and run the sender file.

```
./examples-sender 
```

You should be able to receive the following messages in the first terminal.

```
Received: Hello receiver!
```

**Using Docker**

To automate many steps, you can use the compile script to run a docker container with all the dependencies and commands mentioned above. Note that you will have to create the build/ directory as explained above *before* running the ```compile.sh``` script file with ```mkdir build``` while located in the examples/ directory.

```
./compile.sh
``` 

You might need to give permission to allow execution first

```
sudo chmod +x compile.sh

./compile.sh
``` 

Now you are inside the docker image! You can use the same steps from before to execute the example.

```
cd examples/build
./examples-receiver 
``` 
Start a new terminal window

```
./compile.sh
cd examples/build
./examples-sender
``` 
Go back to the first terminal and you will see:

```
Received: Hello receiver!
``` 
 
##Deploying the Docker image on the car##

#Start by connecting to the car. This you can do by following these steps:
1. Wi-Fi hotspot: BeagleBone-xxxx
2. Password: BeagleBone

#From your ubuntu terminal, test the connection by running:
$ ping 192.168.8.1

#Now ssh to the car by running this command:
$ ssh debian@192.168.8.1

#The ssh requires a password, type the following:
temppwd

#Now that you are in a session inside the car, connect it to the internet by executing the following commands in their order:
1. $ connmanctl
>scan wifi
>services
>agent on
>connect [the wifi id string]
>exit

#To test the internet connection:
$ ping 8.8.8.8

#Pull the image:
# from your car's terminal, where you previously SSH into type:
docker pull seresearch/2018-dit-168:v0.3.5-armhf

#Now that you have the container, Test that it exists by running the following command:
docker ps
#the command should list your pulled containers and you should find the latest one there.

#Thats for deploying.

##for running an executable and communicate with the car##
1. again from the ssh session, Clone the github repository:
git clone https://github.com/Adam2092/DIT168Example
# you might need to install git for the above command.

#Before you can send any commands to the car you need to start the proxies. In the repository you cloned, there is a file named
#docker-compose.yml , this file contains instruction commands for starting the proxies on the car. In order to execute these instructions, run the following:
docker compose [docker-compose.yml]
#the above command will run the configurations in the yml file to start the proxies on the car.


#Now that the proxies are started, find the Example.cpp file, this file has a communication constructer and instatiated objects for sending commands to the car.

#In order to send these commands from the file, you need to be inside DIT168Example and execute the following:
1. mkdir build
2. cd into build
3. cmake ..
4. make

#Now if you ls, you should see the executable "example" in green.
#Run the executable to run the commands on the car by doing the following:
./example

#Thats it for connecting, deploying and communicating with the car. 


