FROM lockblox/orthodox:latest

RUN cd ${VCPKG_ROOT} \
 && git remote add lockblox https://github.com/lockblox/vcpkg.git \
 && git config user.name "engineering" \
 && git config user.email "engineering@lockblox.com" \
 && git pull lockblox master \
 && ./vcpkg install boost-iterator \
 && ./vcpkg install boost-tokenizer \
 && ./vcpkg install boost-range

RUN cd ${VCPKG_ROOT} \
 && rm -rf downloads \
 && ./vcpkg install --head chunx

COPY . /root/src