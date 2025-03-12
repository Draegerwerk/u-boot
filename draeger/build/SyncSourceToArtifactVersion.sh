# This script feteches version information from the artifact deployed by tools\utils\make\tools\localtools.ini
# and subsequently synchronizes the local repository to the commit of the artifact
# Author: auechris
set -e
artifact_version_info_file=$1
repository_path=$2
repository_name=$(basename $repository_path)

if [ -f $artifact_version_info_file ];
then #artifact exists fetch neccessary information
  cd $repository_path
  repository_version=$(git log --pretty=format:'%H' -n 1)
  
  echo Fetchin version information from $artifact_version_info_file
  artifact_version=$(cat ${artifact_version_info_file} | sed -n -e "s/.*$repository_name.*://p")
  echo "The artifact has been build with           GIT-Commit: $artifact_version"
  echo "The currently checked out repository is at GIT-Commit: $repository_version"
  
  if [ "$artifact_version" != "$repository_version" ];
  then #Commits differ, try to checkout the commit that has been used to build the artifact
    echo GIT commits differ
	echo Start to synch repository $repository_name at $repository_path to artifact version.
	echo $(git fetch)
	echo $(git checkout -f $artifact_version)
	#check if checkout worked fine and exit script accordingly
	new_repository_version=$(git log --pretty=format:'%H' -n 1)
	if [ "$artifact_version" == "$new_repository_version" ];
	then
	  exit 0
	else
	  exit 1
	fi
  else #Commits are identical
	echo Both commits are identical. Source synchronization is not required.
	exit 0
  fi  
else #artifact does not exists
  echo Could not find file $artifact_version_info_file
  exit 1
fi