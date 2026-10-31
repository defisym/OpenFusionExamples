# Update this file with OFE when selecting a new FFmpeg SDK.
# SHA256 pins the archive even though the upstream "latest" tag is mutable.
set(OFE_FFMPEG_VERSION "9.0")
set(OFE_FFMPEG_VARIANT "lgpl-shared")
# The release tag can be permanent while the asset keeps its upstream filename.
set(OFE_FFMPEG_RELEASE "latest")
set(OFE_FFMPEG_REPOSITORY "defisym/FFmpeg-Builds-Win32")
set(OFE_FFMPEG_SHA256 "ba7fe27fc6b0925f405783531171061ec831933229c4b6875ebc6ffcf30d2c14")

# Corresponding upstream source for every platform. Android builds this release
# with its NDK; the Windows SHA256 above pins the separately packaged Win32 SDK.
set(OFE_FFMPEG_SOURCE_SHA256 "d97647ace36a307f17ba2bca052d68937487bed8682e1eb9b6737076a9c442b7")
