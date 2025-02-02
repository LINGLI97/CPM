Contextual Pattern Mining and Counting
============================================================

## **Overview**
This repository contains the implementation of several algorithms proposed in our paper `Contextual Pattern Mining and Counting`, including `IM Method`, `EM Method`, `CPC Index`, `CPRI Method`, and `BCPC Index Method`. The build process is managed using GNU Make, with modular rules defined in the Makefile for compiling and cleaning.

---

## **Directory Structure**
```
.
├── EM-SparsePhi-0.2.0/        # lib for external LCP construction of EM
├── include/                   # Header files directory
├── psascan/                   # lib for external SA construction of EM
├── stxxl/                     # STXXL library includes and binaries
├── baseline_mining.cpp        # Source file for IM method
├── ExternalMining.cpp         # Source file for EM method
├── counting.cpp               # Source file for CPC index
├── prefixTree.cpp             # Source file for prefix tree operations
├── ...                        # other source files
├── Makefile.32-bit.gcc        # Makefile for 32-bit compilation
├── Makefile.64-bit.gcc        # Makefile for 64-bit compilation

```

---

## **Dependencies**
### **Required Tools**
- **GCC Compiler** (supporting C++17)
- **GNU Make** (build system)

### **Required Libraries**
- **SDSL Library**:
    - Includes `libsdsl`, `libdivsufsort`, and `libdivsufsort64`.
    - please install [sdsl](https://github.com/simongog/sdsl-lite/tree/master).
    - ```bash
      ./pre-install.sh
    
- **STXXL Library**:
    - please install [STXXL](https://stxxl.org/tags/1.4.1/install_unix.html). 
    - Required for external memory operations.


- **Boost**:
    - please install [Boost](https://www.boost.org/).
    - Required for building range counting data structure.
  
- **EM-SparsePhi-0.2.0**:
    - please install [EM-SparsePhi-0.2.0](https://www.cs.helsinki.fi/group/pads/pSAscan.html).
    - Required for constructing LCP externally.


- **psascan**:
    - please install [psascan](https://www.cs.helsinki.fi/group/pads/better_em_laca.html).
    - Required for constructing SA externally.


- **kkp**:
    - please install [kkp](https://github.com/akiutoslahti/kkp/tree/main).
    - Required for computing the LZ77 factorization used in BCPC index.


​    
Ensure the paths for these libraries are correctly configured in the Makefile.

---

## **Build Instructions**
1. Navigate to the project directory:
   ```bash
   cd /path/to/project
   ```

2. Use the appropriate Makefile depending on your system architecture:
    - For 64-bit systems:
      ```bash
      make -f Makefile.64-bit.gcc
      ```
    - For 32-bit systems:
      ```bash
      make -f Makefile.32-bit.gcc
      ```

3. To build specific components, use the corresponding target:
    - **IM Method**:
      ```bash
      make -f Makefile.64-bit.gcc run_IM
      ```
    - **EM Method**:
      ```bash
      make -f Makefile.64-bit.gcc run_EM
      ```
    - **CPC Index**:
      ```bash
      make -f Makefile.64-bit.gcc run_CI
      ```
    - **CPRI Method**:
      ```bash
      make -f Makefile.64-bit.gcc run_CPRI
      ```
    - **BCPC Index Method**:
      ```bash
      make -f Makefile.64-bit.gcc run_BCPC
      ```

4. R Index competitor
    - Please refer to [here](https://github.com/nicolaprezza/r-index) for the implementation of R Index



### Usage

------
# CPM Problem
#### **1. EM Method**
Run the EM Method with the following command:
```bash
./run_EM -f input.txt -x 6 -y 6 -m 3 -t 1000
```
- **Explanation**:
    - `-f input.txt`: Specifies the input text file to process.
    - `-x 6`: Specifies the context size in one dimension (e.g., rows).
    - `-y 6`: Specifies the context size in another dimension (e.g., columns).
    - `-m 3`: Specifies the minimum pattern length for mining.
    - `-t 1000`: Specifies the frequency threshold for patterns.

#### **2. Baseline Mining**
Run the baseline mining algorithm:
```bash
./run_IM -f input.txt -x 6 -y 6 -m 3 -t 1000
```
- **Explanation**:
    - Similar to `run_EM`, but uses the baseline mining method.
------

# CPC Problem

1.**R-Index method**:
- Build the R-Index:
```bash
./ri-build input.txt
```
- Locate patterns:
```bash
./ri-locate -c input.txt input.txt.ri pattern.txt_r_index 2 2
```
- **Explanation**:
    - `-c input.txt`: The input text file.
    - `input.txt.ri`: The R-Index file generated using `ri-build`.
    - `pattern.txt_r_index`: The file containing patterns for R-Index querying.
    - `2 2`: Context sizes for the query.

2.**Run baseline reporting**:
   ```bash
   ./run_CPRI -f input.txt -p pattern.txt
   ```

3.**CPC Index**:
   ```bash
   ./run_CI -f input.txt -p pattern.txt
   ```
- **Explanation**:
    - `-f input.txt`: Specifies the input text file to process.
    - `-p pattern.txt`: Specifies the file containing patterns to be searched.

4.**BCPC Index**:
   ```bash
   ./run_BCPC -f input.txt -p pattern.txt
   ```
- **Explanation**:
    - Similar to `run_CI`, but uses the BCPC Index method for pattern counting.

    
------
## **Datasets**


The preprocessed datasets are available at [Google Drive](https://drive.google.com/file/d/1-okJkGUUGBtHnpzdk6rLXab8M4ZnU9Pq/view?usp=sharing).

------

## **Notes**
- Make sure the required libraries (SDSL, STXXL and so on) are properly installed and accessible from the paths specified in the Makefile.
- Modify the `Makefile` variables (`CFLAGS`, `LFLAGS`, etc.) if your library paths or compiler configurations differ.


------
License


This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program. If not, see http://www.gnu.org/licenses/.