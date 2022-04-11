FROM rockylinux:latest

RUN dnf -y update 
RUN dnf -y groupinstall "Development Tools"
RUN git clone https://github.com/ugermann/ssplit-cpp.git
WORKDIR ssplit-cpp/build
RUN dnf -y install cmake
RUN cmake -DSSPLIT_USE_INTERNAL_PCRE2=ON ..
RUN make -j
