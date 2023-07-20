# Index of example queries

Most files in this folder were added during development to test some individual
rule of the synthesizer, including many that were copied from the Suslik paper
were they were also used as examples for an individual rule. Below is an
overview for each example stating which rule it is testing, if any, and whether
the current version of DeSyL is able to synthesize it.

| File                  | Tested rule       | Works on current version |
| --------------------- | ----------------- | ------------------------ |
| `empty.sep`           | Empty             | :heavy_check_mark:       |
| `frame.sep`           | Frame             | :heavy_check_mark:       |
| `write.sep`           | Write             | :heavy_check_mark:       |
| `swap.sep`            | Read              | :heavy_check_mark:       |
| `three_swap.sep`      | Read              | :heavy_check_mark:       |
| `four_swap.sep`       | Read              | :heavy_check_mark:       |
| `pick.sep`            | Pick              | :heavy_check_mark:       |
| `three_pick.sep`      | Pick              | :heavy_check_mark:       |
| `notsure.sep`         | HeapUnify         | :heavy_check_mark:       |
| `three_notsure.sep`   | HeapUnify         | :heavy_check_mark:       |
| `elem.sep`            | PureFrame         | :heavy_check_mark:       |
| `fortytwo1.sep`       | PureUnify         | :heavy_check_mark:       |
| `fortytwo2.sep`       | SubstRight        | :heavy_check_mark:       |
| `urk.sep`             | Should fail       | :x:                      |
| `max.sep`             | Branch            | :heavy_check_mark:       |
| `maxx.sep`            | Branch            | :heavy_check_mark:       |
| `hilo.sep`            | Branch            | :hourglass_flowing_sand: |
| `hilodif.sep`         | Branch            | :hourglass_flowing_sand: |
| `three_max.sep`       | Should fail       | :x:                      |
| `predicate_parse.sep` | Predicate parsing | :heavy_check_mark:       |
| `open.sep`            | Open              | :x:                      |
| `listfree.sep`        | Free              | :x:                      |