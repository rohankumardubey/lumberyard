![lmbr](http://d2tinsms4add52.cloudfront.net/github/readme_header.jpg)   

------------

**Amazon Lumberyard is no longer offered. This archive provides source for convenience only; you can also view the For continued support, visit our [documentation archive on GitHub](https://github.com/awsdocs/amazon-lumberyard-user-guide/) or [contact Lumberyard customer support ](https://pages.awscloud.com/Amazon-Game-Tech-Contact-Us.html). We recommend Open 3D Engine, Lumberyard’s Apache-licensed successor. O3DE is maintained by the open-source community, including developers from Adobe, AWS, Epic, Microsoft, Intel, Lightspeed Studios, Niantic, and others. Visit O3DE's [website](https://www.o3de.org/ "website"), [Discord](https://discord.gg/o3de), and [Github](https://github.com/o3de/o3de).**

------------
 
# Amazon Lumberyard
Amazon Lumberyard is a free, AAA game engine that gives you the tools you need to create high quality games.

## Acquiring Lumberyard source 
Each release of Lumberyard exists as a separate branch in GitHub. You can get Lumberyard from GitHub using the following steps:

### Fork the repository
Forking creates a copy of the Lumberyard repository in your GitHub account. Your fork becomes the remote repository into which you can push changes.

### Create a branch
The GitHub workflow assumes your master branch is always deployable. Create a branch for your local project or fixes.

For more information about branching, see the [GitHub documentation](https://guides.github.com/introduction/flow/).

### Clone the repository
Cloning the repository copies your fork onto your computer. To clone the repository, click the "Clone or download" button on the GitHub website, and copy the resultant URL to the clipboard. In a command line window, type ```git clone [URL]```, where ```[URL]``` is the URL that you copied in the previous step.

For more information about cloning a repository, see the [GitHub documentation](https:// help.github.com/articles/cloning-a-repository/).


### Downloading additive  files
Once the repository exists locally on your machine, manually execute ```git_bootstrap.exe``` found at the root of the repository. This application will perform a download operation for __Lumberyard binaries that are required prior to using or building the engine__. This program uses AWS services to download the binaries. Monitor the health of AWS services on the [AWS Service Health Dashboard](https://status.aws.amazon.com/).

### Running the Setup Assistant
```git_bootstrap.exe``` will launch the Setup Assistant when it completes. Setup Assistant lets you configure your environment and launch the Lumberyard Editor.

## Lumberyard Documentation
Full Lumberyard documentation can be found here:
https://github.com/awsdocs/amazon-lumberyard-user-guide/ 
<br>We also have tutorials available at https://www.youtube.com/c/AmazonLumberyardTutorials/

## License
Your use of Lumberyard is governed by the AWS Customer Agreement at https://aws.amazon.com/agreement/ and Lumberyard Service Terms at https://aws.amazon.com/serviceterms/#57._Amazon_Lumberyard_Engine.

For complete copyright and license terms please see the LICENSE.txt file at the root of this distribution (the "License").  As a reminder, here are some key pieces to keep in mind when submitting changes/fixes and creating your own forks:
-	If you submit a change/fix, we can use it without restriction, and other Lumberyard users can use it under the License.
-	Only share forks in this GitHub repo (i.e., forks must be parented to https://github.com/aws/lumberyard).
-	Your forks are governed by the License, and you must include the License.txt file with your fork.  Please also add a note at the top explaining your modifications.
-	If you use someone else’s fork from this repo, your use is subject to the License.    
-	Your fork may not enable the use of third-party compute, storage or database services.  
-	It's fine to connect to third-party platform services like Steamworks, Apple GameCenter, console platform services, etc.  
To learn more, please see our FAQs https://aws.amazon.com/lumberyard/faq/#licensing.
