#!/bin/sh
set -eu
cd "$(dirname "$0")"
mkdir -p ThirdParty
fetch() {
  dep_name="$1"
  dep_url="$2"
  dep_commit="$3"
  if [ ! -d "ThirdParty/$dep_name/.git" ]; then
    git init "ThirdParty/$dep_name"
    git -C "ThirdParty/$dep_name" remote add origin "$dep_url"
  fi
  git -C "ThirdParty/$dep_name" fetch --depth 1 origin "$dep_commit"
  git -C "ThirdParty/$dep_name" checkout --detach FETCH_HEAD
}
fetch JUCE https://github.com/juce-framework/JUCE.git f72bad64d29715216226685810c5196bd0d79d77
fetch linear https://github.com/Signalsmith-Audio/linear.git 7bb1c8f36794d6980080ce817c91f8da8fe56a66
fetch signalsmith-stretch https://github.com/Signalsmith-Audio/signalsmith-stretch.git 88061fd4eda56a2353c5d65c74aa3f4ccf76f9da
