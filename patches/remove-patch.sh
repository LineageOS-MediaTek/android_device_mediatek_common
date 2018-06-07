echo $1
rootdirectory="$PWD"
dirs="system/core"

for dir in $dirs ; do
	cd $rootdirectory
	cd $dir
	echo "Reverting $dir patches..."
	git checkout -- .
	git clean -fd
	echo " "
done

echo "Changing to build directory..."
cd $rootdirectory
