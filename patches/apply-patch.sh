echo $1
rootdirectory="$PWD"
dirs="system/core"

for dir in $dirs ; do
	cd $rootdirectory
	cd $dir
	echo "Applying $dir patches..."
	git apply $rootdirectory/device/mediatek/mt6737-common/patches/$dir/*.patch
done

echo "Changing to build directory..."
cd $rootdirectory
