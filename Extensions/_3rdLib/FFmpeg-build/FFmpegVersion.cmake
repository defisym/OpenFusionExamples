# Update this file with OFE when selecting a new FFmpeg SDK.
# SHA256 pins the archive even though the upstream "latest" tag is mutable.
set(OFE_FFMPEG_VERSION "9.0")
set(OFE_FFMPEG_VARIANT "lgpl-shared")
# The release tag can be permanent while the asset keeps its upstream filename.
set(OFE_FFMPEG_RELEASE "latest")
set(OFE_FFMPEG_REPOSITORY "defisym/FFmpeg-Builds-Win32")
set(OFE_FFMPEG_SHA256 "ba7fe27fc6b0925f405783531171061ec831933229c4b6875ebc6ffcf30d2c14")
