# Contribution after cloning

When contributing to this project, there are a few guidelines to keep in mind; readability, coherence and maintainability and passing status checks. 

Each part that you contribute with should be clear and self explanatory meaning that your code for example should be easy to read and commented very well. 
Unless your account is added as a collaborator to this repository, you are going to have to fork it, create a new branch and  and start developing and pushing code to that branch with brief and understandable commit messages.

When you are done, you will create a pull request from your new branch to the base branch in the original repository. Your pull request has to include a pull request comment explaining what changes you made and why. Your pull request also has to pass the status checks with whatever CI tool we are going to be using at the time. And most importantly make sure that your branch is updated with the forked repository, because if you there are files/lines of code changed other than the ones mentioned in your pull request then your pull request will be denied and closed since we are trying to adhere with principles that fit a professional enterprise industrial project. 

**package and release the binaries using Docker**

After your pull request is accepted, the repository will have a docker file that specifies the appropriate working directory and the need dependencies and commands. So all you have to do is make sure that your local clone is updated with the remote repository, and then run the compile script as a start.

```
./compile.sh
``` 

You might need to give permission to allow execution first

```
sudo chmod +x compile.sh

./compile.sh
``` 

Then you need to save your docker image and then you could either push it to your Docker hub account or export it to a compressed file

```
docker save myrepository/mydockerimage > FILENAME.tar
```

Which makes everything set to be uploaded to the miniature car!
