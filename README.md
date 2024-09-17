Contextual Pattern Matching
===

How to use
----------

### Installation


Before compiling and running, please install [sdsl](https://github.com/simongog/sdsl-lite/tree/master).
```bash
./pre-install.sh
```
### Counting
make command:
```
make run_counting
```
Running:
```
 ./run_counting -f input.txt -p pattern.txt -x 1 -y 2
```
The `-f` option determines the input file path;   
The `-p` option determines the pattern file path of P in the given P algorithm.
The `-l` option determines the length of the context.
The `-x` and `-y` option determine the length of context X and Y.
### Baseline

```


 ./CPM -f input.txt -p pattern.txt -l 2 -m 3


The `-f` option determines the input file path;   
The `-p` option determines the pattern file path of P in the given P algorithm.
The `-l` option determines the length of the context.
The `-m` option determines the length of pattern P in the mining algorithm.
 
```


### Test the correctness
If you would like to compile the test.cpp to test the consistence between the results of the baseline and naive checker we implemented:
```
make -f Makefile.32-bit.gcc run_test
```
or:
```
make -f Makefile.64-bit.gcc run_test
```
Then:
```
./run_test -L 3 -R 2000 -r 100000

```
The `-L` option determines the minimal length of randomly generated text string;  
The `-R` option determines the maximal length of randomly generated text string.  
The `-r` option determines the number of test rounds.  



Once the inconsistency is found, the specific string and their results will be printed.


This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.