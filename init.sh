# Custom Chipyard Setup
git clone https://github.com/jfwang983/power-mappings-chipyard.git
cd power-mappings-chipyard
git checkout latest-version
./build-setup.sh riscv-tools -s 4 -s 6 -s 7 -s 8 -s 9 -f
source env.sh
source /ecad/tools/vlsi.bashrc

# Hammer Setup
cd ../hammer
pip install -e .

# Gemmini Setup
cd ../power-mappings-chipyard/generators/gemmini
git config remote.origin.fetch "+refs/heads/*:refs/remotes/origin/*"
git fetch && git checkout v0.7.1
git submodule update --init --recursive
make -C software/libgemmini install

# Gemmini-DOSA Setup
cd software/gemmini-rocc-tests
git checkout power

