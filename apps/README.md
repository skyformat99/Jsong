# Jsong vs. RapidJSON Benchmark

Date: **31.07.2017**

OS: **Windows 10 (1703)** 64-bit

Compiler: **gcc 6.3.0** (x86_64-win32-seh-rev1, Built by MinGW-W64 project)

CPU: **Intel Core i5-2400**

Jsong: **1.0 RC**

RapidJSON: **1.1**

* * *

Lowest measurement was recorded after about a dozen of runs.

**Jsong** suffixes explanation:

  * `z` denotes parsing from an implicit (null-terminated) string.
  * `b` denotes parsing from an explicit string.
  * `s` denotes parsing in streaming mode.

**RapidJSON** tests always parse from a null-terminated string (fastest).

The second set of RapidJSON entries is for non-recursive
variant (`kParseIterativeFlag`), which prevents stack overflow
but happens to be slower, for whatever reason.

JSON sample files are from [here](https://github.com/garnetius/json-samples).

## SSE4.2

Relevant compiler switches: `-march=native -DRAPIDJSON_SSE42`.

**github.json**:

Small file with several GitHub events encoded in JSON.

```
dombenchz: 11.587555us [35]
dombenchb: 12.911847us [39]
dombenchs: 21.188671us [64]

saxbenchz: 8.938971us [27]
saxbenchb: 12.580774us [38]
saxbenchs: 10.925409us [33]

rapiddom: 31.451934us [95]
rapidsax: 24.830474us [75]

rapiddom: 36.749053us [111]
rapidsax: 27.810094us [84]
```

**generated.json**:

Larger file with lots of entries.
Every kind of JSON value is represented.

```
dombenchz: 143.023531us [432]
dombenchb: 129.449495us [391]
dombenchs: 176.792975us [534]

saxbenchz: 97.004353us [293]
saxbenchb: 113.889108us [344]
saxbenchs: 115.213400us [348]

rapiddom: 182.752289us [552]
rapidsax: 198.312720us [599]

rapiddom: 225.460406us [681]
rapidsax: 244.993686us [740]
```

**catalog.json**:

Big music (?) catalogue with many objects and lots of whitespace.

```
dombenchz: 1931.810883us [5835]
dombenchb: 2161.906610us [6530]
dombenchs: 2337.706366us [7061]

saxbenchz: 1191.531683us [3599]
saxbenchb: 1442.485007us [4357]
saxbenchs: 1555.380896us [4698]

rapiddom: 2400.610234us [7251]
rapidsax: 1917.243672us [5791]

rapiddom: 3511.355457us [10606]
rapidsax: 3207.761930us [9689]
```

**laptop.json**:

Laptop 3D model encoded in JSON.

```
dombenchz: 5461.380005us [16496]
dombenchb: 6154.977914us [18591]
dombenchs: 6946.242355us [20981]

saxbenchz: 3590.155478us [10844]
saxbenchb: 4056.306245us [12252]
saxbenchs: 4989.600997us [15071]

rapiddom: 6592.987477us [19914]
rapidsax: 4230.119564us [12777]

rapiddom: 7938.126513us [23977]
rapidsax: 5603.402896us [16925]
```

**canada.json**:

Huge array of coordinates from the map of Canada.

```
dombenchz: 4742.287906us [14324]
dombenchb: 5075.678294us [15331]
dombenchs: 6539.682561us [19753]

saxbenchz: 3444.151151us [10403]
saxbenchb: 4540.333450us [13714]
saxbenchs: 5265.051980us [15903]

rapiddom: 6884.331706us [20794]
rapidsax: 4766.457804us [14397]

rapiddom: 9162.432801us [27675]
rapidsax: 7293.528260us [22030]
```

**ascii.json**:

A document where each JSON string character was encoded
as a Unicode sequence for the purpose of making it compatible
with legacy software, which could only process ASCII data.

```
dombenchz: 13312.444835us [40210]
dombenchb: 13997.434846us [42279]
dombenchs: 15265.444389us [46109]

saxbenchz: 12676.784699us [38290]
saxbenchb: 14171.248165us [42804]
saxbenchs: 14513.577634us [43838]

rapiddom: 15093.948582us [45591]
rapidsax: 18006.728727us [54389]

rapiddom: 16423.515951us [49607]
rapidsax: 17746.150785us [53602]
```

**citylots.json**:

Huge 180 MB (one hundred and eighty megabytes) JSON document
describing something in the city of San Francisco
(the whole damn city apparently).

```
dombenchz: 463230.089462us [1399180]
dombenchb: 505751.392326us [1527615]
dombenchs: 600239.630628us [1813013]

saxbenchz: 306911.205680us [927021]
saxbenchb: 382885.910262us [1156500]
saxbenchs: 430190.611962us [1299383]

rapiddom: 542915.004956us [1639865]
rapidsax: 421618.470165us [1273491]

rapiddom: 695674.139857us [2101274]
rapidsax: 568103.609816us [1715949]
```

## SSE2

Relevant compiler switches: `-mtune=core2 -mno-sse4.2 -DRAPIDJSON_SSE2`.

**github.json**:

```
dombenchz: 10.925409us [33]
dombenchb: 13.242920us [40]
dombenchs: 18.871160us [57]

saxbenchz: 8.607898us [26]
saxbenchb: 13.905065us [42]
saxbenchs: 11.587555us [35]

rapiddom: 30.458715us [92]
rapidsax: 24.499401us [74]

rapiddom: 34.431545us [104]
rapidsax: 27.810094us [84]
```

**generated.json**:

```
dombenchz: 140.043874us [423]
dombenchb: 136.733144us [413]
dombenchs: 170.502589us [515]

saxbenchz: 94.686874us [286]
saxbenchb: 109.585159us [331]
saxbenchs: 110.578378us [334]

rapiddom: 179.772632us [543]
rapidsax: 198.312720us [599]

rapiddom: 218.838955us [661]
rapidsax: 243.669396us [736]
```

**catalog.json**:

```
dombenchz: 1983.127196us [5990]
dombenchb: 2089.070552us [6310]
dombenchs: 2271.491769us [6861]

saxbenchz: 1227.618638us [3708]
saxbenchb: 1550.083728us [4682]
saxbenchs: 1642.122019us [4960]

rapiddom: 2408.887058us [7276]
rapidsax: 1913.601869us [5780]

rapiddom: 9026.030910us [27263]
rapidsax: 6986.292934us [21102]
```

**laptop.json**:

```
dombenchz: 5678.894958us [17153]
dombenchb: 6327.135868us [19111]
dombenchs: 7220.370789us [21809]

saxbenchz: 3821.244424us [11542]
saxbenchb: 4306.928497us [13009]
saxbenchs: 4996.222457us [15091]

rapiddom: 6576.764900us [19865]
rapidsax: 4225.484542us [12763]

rapiddom: 7900.715315us [23864]
rapidsax: 5382.908578us [16259]
```

**canada.json**:

```
dombenchz: 5283.924884us [15960]
dombenchb: 5670.949206us [17129]
dombenchs: 6794.279853us [20522]

saxbenchz: 3996.713107us [12072]
saxbenchb: 5070.713879us [15316]
saxbenchs: 5299.816387us [16008]

rapiddom: 6880.027757us [20781]
rapidsax: 4942.257560us [14928]

rapiddom: 9048.874916us [27332]
rapidsax: 6839.296722us [20658]
```

**ascii.json**:

```
dombenchz: 13449.840125us [40625]
dombenchb: 14302.684141us [43201]
dombenchs: 14510.597977us [43829]

saxbenchz: 12125.879247us [36626]
saxbenchb: 13546.182364us [40916]
saxbenchs: 13334.626725us [40277]

rapiddom: 15683.920646us [47373]
rapidsax: 17241.287980us [52077]

rapiddom: 15947.102552us [48168]
rapidsax: 18880.074266us [57027]
```

**citylots.json**:

```
dombenchz: 485059.007139us [1465112]
dombenchb: 533188.742724us [1610487]
dombenchs: 626506.961472us [1892353]

saxbenchz: 312320.351520us [943358]
saxbenchb: 430521.353877us [1300382]
saxbenchs: 434082.043859us [1311137]

rapiddom: 549166.325110us [1658747]
rapidsax: 426723.284562us [1288910]

rapiddom: 684381.586275us [2067165]
rapidsax: 567293.475288us [1713502]
```

## Generic

Relevant compiler switches: `-mtune=generic -mno-sse2`.

**github.json**:

```
dombenchz: 15.560430us [47]
dombenchb: 20.857598us [63]
dombenchs: 24.830474us [75]

saxbenchz: 13.573992us [41]
saxbenchb: 19.202233us [58]
saxbenchs: 16.553649us [50]

rapiddom: 31.451934us [95]
rapidsax: 24.499401us [74]

rapiddom: 36.749053us [111]
rapidsax: 27.479022us [83]
```

**generated.json**:

```
dombenchz: 143.685677us [434]
dombenchb: 165.205421us [499]
dombenchs: 208.244909us [629]

saxbenchz: 103.294772us [312]
saxbenchb: 137.726363us [416]
saxbenchs: 146.996407us [444]

rapiddom: 179.441559us [542]
rapidsax: 188.380530us [569]

rapiddom: 215.859302us [652]
rapidsax: 225.129333us [680]
```

**catalog.json**:

```
dombenchz: 2415.839591us [7297]
dombenchb: 2973.035429us [8980]
dombenchs: 3261.400002us [9851]

saxbenchz: 1875.197402us [5664]
saxbenchb: 2371.806884us [7164]
saxbenchs: 2475.763802us [7478]

rapiddom: 3301.459833us [9972]
rapidsax: 2872.058168us [8675]

rapiddom: 4512.518846us [13630]
rapidsax: 4150.325477us [12536]
```

**laptop.json**:

```
dombenchz: 5756.027341us [17386]
dombenchb: 6448.639654us [19478]
dombenchs: 7125.021768us [21521]

saxbenchz: 3648.755397us [11021]
saxbenchb: 4076.832770us [12314]
saxbenchs: 4646.609382us [14035]

rapiddom: 6736.673153us [20348]
rapidsax: 4236.078877us [12795]

rapiddom: 7947.727617us [24006]
rapidsax: 5180.623251us [15648]
```

**canada.json**:

```
dombenchz: 5116.070879us [15453]
dombenchb: 5591.491689us [16889]
dombenchs: 6080.817565us [18367]

saxbenchz: 3655.376857us [11041]
saxbenchb: 4542.652464us [13721]
saxbenchs: 5011.782888us [15138]

rapiddom: 7180.973103us [21690]
rapidsax: 5105.145470us [15420]

rapiddom: 7878.202382us [23796]
rapidsax: 8947.235644us [27025]
```

**ascii.json**:

```
dombenchz: 11955.045585us [36110]
dombenchb: 12965.480344us [39162]
dombenchs: 13442.887592us [40604]

saxbenchz: 10688.360335us [32284]
saxbenchb: 11686.876465us [35300]
saxbenchs: 12786.369858us [38621]

rapiddom: 15261.802586us [46098]
rapidsax: 17062.839640us [51538]

rapiddom: 15456.121962us [46685]
rapidsax: 18232.496360us [55071]
```

**citylots.json**:

```
dombenchz: 471201.285093us [1423255]
dombenchb: 553340.493338us [1671355]
dombenchs: 635096.319064us [1918297]

saxbenchz: 299925.641007us [905920]
saxbenchb: 373218.910094us [1127301]
saxbenchs: 427220.225116us [1290411]

rapiddom: 551524.558001us [1665870]
rapidsax: 428399.176026us [1293972]

rapiddom: 718139.067686us [2169129]
rapidsax: 618568.998499us [1868379]
```

## Observations

  * **Jsong** manages to outperform **RapidJSON** in all tests.

  * Surprisingly, the SAX API of RapidJSON turned out to perform quite poorly.
    I actually expected the opposite. Not only it is much slower than SAX parsing
    of Jsong, it is also sometimes slower than its own DOM parser and on occasion
    *it even loses to Jsong DOM parser*, which is ridiculous. There’s definitely
    a room for improvement because these numbers are absurd.

  * Enabling SSE positively affects the performance, especially for documents
    with lots of strings.

    RapidJSON SSE4.2 code doesn’t appear to be any faster than its SSE2 code.
    Jsong SSE4.2 code seems to perform slightly better than its SSE2 code.
