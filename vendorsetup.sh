cd frameworks/av
git reset --hard && git clean -f -d
patch -p1 < ../../device/mediatek/common/patches/001-Add-MTKEngineerMode-support-av.patch
cd ../..
cd frameworks/base
git reset --hard && git clean -f -d
patch -p1 < ../../device/mediatek/common/patches/002-Add-MTKEngineerMode-support-base.patch
cd ../..


