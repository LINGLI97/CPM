Contextual Pattern Matching
===

How to use
----------

### Installation


Before compiling and running, please install [sdsl](https://github.com/simongog/sdsl-lite/tree/master).
```bash
./pre-install.sh
```
Using int32_t:
```
make -f Makefile.32-bit.gcc
```
Using int64_t:

```
make -f Makefile.64-bit.gcc

```

### Usage

```
 ./CPM -f input.txt -p pattern.txt -l 2


The `-f` option determines the input file path;   
The `-p` option determines the pattern file path.
The `-l` option determines the length of the context.
 
```

---------------

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.