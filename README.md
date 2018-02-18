# Group 7 - DIT168

This is a repository for completing the assignments of the DIT168 V18 Industrial IT and Embedded Systems course

# Authors 🖋️

Måns Thörnvik, Rashad Kamsheh, Madisen Whitfield and Sarah Aldelame.

# Introduction

We -Group 7- will be using GitHub in a very professional manner, giving high priority to traceability, shared understanding, code co- ownership, maintainable version control and secure backing up. And in order to adhere to these principles, the team will agree to follow regulations related to the following repository layout:

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

**How to clone, build, and test the project**

Using your terminal or git bash/interface, clone this repository using this command:

```
git clone https://github.com/Rashadse/Group-7---DIT168.git
```

change directory into the examples folder 

```
cd Group-7---DIT168/examples/
```

make the build folder and move into it

```
mkdir build && cd build/
```

generate a Makefile

```
cmake ..
```

Now try the make command, you should see a "[100%] Built target" line at the end
```
make
```

Now run make test to check if the example passes all the tests

```
make test
``` 

