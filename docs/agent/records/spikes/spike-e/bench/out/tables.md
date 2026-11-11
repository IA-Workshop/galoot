### GCC 16

Per transform, median [range] over 11 rounds at environment size 0; ns at sizes 0 / 1352 / 2768.

| Operation | Loop | Form | ns, sizes 0 / 1352 / 2768 | cycles [range] | instructions |
| --- | --- | --- | --- | --- | ---: |
| point | throughput | galoot | 8.91 / 8.50 / 8.49 | 50.7 [0.1] | 209.0 |
| point | throughput | a | 7.63 / 7.27 / 7.29 | 43.4 [0.1] | 151.0 |
| point | throughput | b | 6.55 / 6.26 / 6.26 | 37.3 [0.1] | 148.0 |
| point | throughput | bp | 7.42 / 7.15 / 7.13 | 42.5 [0.5] | 149.1 |
| point | throughput | c | 6.05 / 5.76 / 5.76 | 34.4 [0.0] | 88.0 |
| point | latency | galoot | 15.57 / 14.90 / 14.92 | 88.8 [0.4] | 204.0 |
| point | latency | a | 9.63 / 9.23 / 9.19 | 54.8 [0.2] | 148.1 |
| point | latency | b | 9.98 / 9.53 / 9.59 | 56.8 [0.1] | 144.1 |
| point | latency | bp | 9.56 / 9.12 / 9.15 | 54.4 [0.2] | 141.1 |
| point | latency | c | 5.59 / 5.35 / 5.35 | 31.8 [0.1] | 79.0 |
| point | batch | galoot | 8.73 / 8.33 / 8.32 | 49.8 [0.0] | 204.0 |
| point | batch | a | 9.73 / 9.26 / 9.27 | 55.4 [0.0] | 147.0 |
| point | batch | b | 4.60 / 4.39 / 4.39 | 26.2 [0.1] | 43.1 |
| point | batch | bp | 5.15 / 4.91 / 4.94 | 29.3 [0.4] | 48.1 |
| point | batch | c | 5.47 / 5.21 / 5.22 | 31.2 [0.1] | 73.0 |
| line | throughput | galoot | 10.10 / 9.63 / 9.65 | 57.5 [0.5] | 266.0 |
| line | throughput | a | 8.54 / 8.15 / 8.14 | 48.7 [0.1] | 227.0 |
| line | throughput | b | 11.79 / 11.25 / 11.29 | 67.2 [0.1] | 245.0 |
| line | throughput | bp | 9.75 / 9.31 / 9.31 | 55.5 [22.1] | 256.1 |
| line | throughput | c | 11.08 / 10.71 / 10.64 | 63.1 [1.5] | 170.0 |
| line | latency | galoot | 14.91 / 14.21 / 14.21 | 85.0 [0.2] | 260.0 |
| line | latency | a | 12.38 / 11.82 / 11.83 | 70.5 [0.3] | 222.1 |
| line | latency | b | 13.32 / 12.71 / 12.70 | 75.9 [0.1] | 257.1 |
| line | latency | bp | 13.10 / 12.49 / 12.49 | 74.6 [0.1] | 255.1 |
| line | latency | c | 11.50 / 11.00 / 10.99 | 65.5 [0.3] | 172.1 |
| line | batch | galoot | 9.95 / 9.49 / 9.48 | 56.8 [0.3] | 260.0 |
| line | batch | a | 12.01 / 11.42 / 11.43 | 68.4 [0.3] | 229.0 |
| line | batch | b | 9.14 / 8.71 / 8.70 | 52.1 [0.1] | 93.2 |
| line | batch | bp | 9.12 / 8.69 / 8.69 | 52.0 [0.1] | 94.2 |
| line | batch | c | 9.72 / 9.34 / 9.33 | 55.4 [1.0] | 107.1 |
| plane | throughput | galoot | 8.27 / 7.94 / 7.92 | 47.2 [0.3] | 180.0 |
| plane | throughput | a | 8.56 / 8.20 / 8.23 | 48.9 [0.7] | 151.0 |
| plane | throughput | b | 7.29 / 7.01 / 7.00 | 41.5 [1.1] | 147.1 |
| plane | throughput | bp | 6.83 / 6.52 / 6.54 | 38.9 [0.0] | 145.1 |
| plane | throughput | c | 6.40 / 6.12 / 6.12 | 36.5 [0.0] | 109.1 |
| plane | latency | galoot | 13.18 / 12.58 / 12.56 | 75.0 [0.1] | 175.0 |
| plane | latency | a | 9.87 / 9.41 / 9.41 | 56.2 [0.6] | 146.1 |
| plane | latency | b | 7.07 / 6.76 / 6.75 | 40.3 [0.1] | 136.0 |
| plane | latency | bp | 7.06 / 6.76 / 6.75 | 40.2 [0.1] | 136.1 |
| plane | latency | c | 8.27 / 7.93 / 7.96 | 47.2 [0.3] | 105.1 |
| plane | batch | galoot | 8.10 / 7.72 / 7.72 | 46.2 [0.2] | 175.0 |
| plane | batch | a | 8.44 / 8.04 / 8.04 | 48.1 [0.1] | 145.0 |
| plane | batch | b | 4.91 / 4.68 / 4.68 | 28.0 [0.1] | 49.1 |
| plane | batch | bp | 7.29 / 6.95 / 6.95 | 41.5 [0.0] | 48.1 |
| plane | batch | c | 7.90 / 7.53 / 7.53 | 45.0 [0.0] | 65.1 |

### Clang 23

Per transform, median [range] over 11 rounds at environment size 0; ns at sizes 0 / 1352 / 2768.

| Operation | Loop | Form | ns, sizes 0 / 1352 / 2768 | cycles [range] | instructions |
| --- | --- | --- | --- | --- | ---: |
| point | throughput | galoot | 8.54 / 8.37 / 8.38 | 50.0 [0.1] | 189.1 |
| point | throughput | a | 5.78 / 5.68 / 5.67 | 33.7 [0.0] | 166.4 |
| point | throughput | b | 5.47 / 5.39 / 5.38 | 32.1 [0.0] | 147.5 |
| point | throughput | bp | 5.41 / 5.37 / 5.36 | 32.0 [0.1] | 134.6 |
| point | throughput | c | 3.44 / 3.41 / 3.41 | 20.3 [0.0] | 82.8 |
| point | latency | galoot | 8.28 / 8.20 / 8.19 | 48.9 [0.3] | 181.1 |
| point | latency | a | 8.75 / 8.66 / 8.65 | 51.7 [0.0] | 161.0 |
| point | latency | b | 5.40 / 5.28 / 5.27 | 31.3 [0.1] | 140.0 |
| point | latency | bp | 4.90 / 4.83 / 4.82 | 28.5 [0.0] | 148.0 |
| point | latency | c | 4.87 / 4.84 / 4.85 | 28.8 [0.1] | 76.0 |
| point | batch | galoot | 8.26 / 8.18 / 8.18 | 49.0 [0.1] | 185.0 |
| point | batch | a | 2.75 / 2.72 / 2.71 | 16.2 [0.0] | 72.6 |
| point | batch | b | 0.89 / 0.88 / 0.87 | 5.2 [0.0] | 23.2 |
| point | batch | bp | 0.89 / 0.87 / 0.88 | 5.2 [0.0] | 23.2 |
| point | batch | c | 1.48 / 1.45 / 1.46 | 8.7 [0.0] | 37.1 |
| line | throughput | galoot | 10.86 / 10.69 / 10.71 | 63.6 [0.2] | 298.1 |
| line | throughput | a | 8.04 / 7.92 / 7.91 | 47.0 [0.0] | 233.2 |
| line | throughput | b | 8.71 / 8.54 / 8.54 | 50.8 [0.0] | 247.1 |
| line | throughput | bp | 8.89 / 8.72 / 8.73 | 51.9 [0.0] | 246.2 |
| line | throughput | c | 6.05 / 5.96 / 5.94 | 35.4 [0.0] | 170.4 |
| line | latency | galoot | 14.52 / 13.86 / 13.83 | 82.7 [0.1] | 289.1 |
| line | latency | a | 11.52 / 11.00 / 10.99 | 65.6 [0.1] | 244.0 |
| line | latency | b | 9.27 / 9.18 / 9.20 | 54.8 [0.0] | 222.0 |
| line | latency | bp | 9.17 / 9.06 / 9.07 | 54.1 [0.0] | 215.0 |
| line | latency | c | 6.30 / 6.20 / 6.20 | 36.7 [0.1] | 179.0 |
| line | batch | galoot | 11.06 / 10.57 / 10.56 | 63.0 [0.2] | 292.0 |
| line | batch | a | 4.42 / 4.28 / 4.27 | 25.5 [0.7] | 118.6 |
| line | batch | b | 1.78 / 1.74 / 1.73 | 10.2 [0.0] | 52.2 |
| line | batch | bp | 1.78 / 1.73 / 1.73 | 10.2 [0.1] | 51.7 |
| line | batch | c | 2.52 / 2.42 / 2.42 | 14.4 [0.1] | 64.6 |
| plane | throughput | galoot | 13.57 / 12.94 / 13.04 | 77.3 [1.8] | 190.1 |
| plane | throughput | a | 5.60 / 5.35 / 5.34 | 31.8 [0.0] | 152.5 |
| plane | throughput | b | 4.91 / 4.70 / 4.69 | 27.9 [0.2] | 128.6 |
| plane | throughput | bp | 5.82 / 5.55 / 5.56 | 33.1 [0.0] | 129.6 |
| plane | throughput | c | 4.12 / 3.93 / 3.94 | 23.4 [0.0] | 106.7 |
| plane | latency | galoot | 12.42 / 11.93 / 11.86 | 70.7 [2.0] | 182.1 |
| plane | latency | a | 6.58 / 6.29 / 6.28 | 37.5 [0.3] | 149.0 |
| plane | latency | b | 4.93 / 4.72 / 4.72 | 28.1 [0.0] | 123.0 |
| plane | latency | bp | 5.98 / 5.73 / 5.71 | 34.0 [0.0] | 140.0 |
| plane | latency | c | 4.55 / 4.35 / 4.35 | 25.9 [0.0] | 105.0 |
| plane | batch | galoot | 13.58 / 13.02 / 13.03 | 77.5 [2.6] | 186.0 |
| plane | batch | a | 2.79 / 2.67 / 2.67 | 15.9 [0.2] | 75.6 |
| plane | batch | b | 0.92 / 0.88 / 0.88 | 5.2 [0.0] | 23.2 |
| plane | batch | bp | 0.91 / 0.88 / 0.88 | 5.2 [0.0] | 23.7 |
| plane | batch | c | 1.29 / 1.28 / 1.28 | 7.6 [0.0] | 31.6 |

### The criterion, pair by pair

Ratio of medians (first form over second) at size 0, in ns and in cycles; the form named is faster beyond the criterion at all three sizes, '-' when not.

| Compiler | Operation | Loop | Pair | ns ratio | ns verdict | cycles ratio | cycles verdict |
| --- | --- | --- | --- | ---: | --- | ---: | --- |
| gcc | point | throughput | b / a | 0.859 | b | 0.859 | b |
| gcc | point | throughput | bp / a | 0.973 | - | 0.978 | - |
| gcc | point | throughput | c / a | 0.793 | c | 0.793 | c |
| gcc | point | throughput | c / b | 0.923 | - | 0.923 | c |
| gcc | point | throughput | bp / b | 1.133 | b | 1.138 | b |
| gcc | point | throughput | a / galoot | 0.856 | a | 0.856 | a |
| gcc | point | throughput | b / galoot | 0.735 | b | 0.735 | b |
| gcc | point | throughput | c / galoot | 0.678 | c | 0.678 | c |
| gcc | point | latency | b / a | 1.037 | - | 1.036 | a |
| gcc | point | latency | bp / a | 0.992 | - | 0.992 | - |
| gcc | point | latency | c / a | 0.581 | c | 0.580 | c |
| gcc | point | latency | c / b | 0.560 | c | 0.560 | c |
| gcc | point | latency | bp / b | 0.957 | - | 0.957 | bp |
| gcc | point | latency | a / galoot | 0.618 | a | 0.618 | a |
| gcc | point | latency | b / galoot | 0.641 | b | 0.640 | b |
| gcc | point | latency | c / galoot | 0.359 | c | 0.359 | c |
| gcc | point | batch | b / a | 0.473 | b | 0.473 | b |
| gcc | point | batch | bp / a | 0.530 | bp | 0.529 | bp |
| gcc | point | batch | c / a | 0.562 | c | 0.563 | c |
| gcc | point | batch | c / b | 1.190 | b | 1.191 | b |
| gcc | point | batch | bp / b | 1.121 | - | 1.119 | b |
| gcc | point | batch | a / galoot | 1.114 | galoot | 1.114 | galoot |
| gcc | point | batch | b / galoot | 0.526 | b | 0.527 | b |
| gcc | point | batch | c / galoot | 0.627 | c | 0.627 | c |
| gcc | line | throughput | b / a | 1.381 | a | 1.381 | a |
| gcc | line | throughput | bp / a | 1.141 | - | 1.140 | - |
| gcc | line | throughput | c / a | 1.297 | a | 1.296 | a |
| gcc | line | throughput | c / b | 0.939 | - | 0.938 | c |
| gcc | line | throughput | bp / b | 0.826 | - | 0.825 | - |
| gcc | line | throughput | a / galoot | 0.846 | a | 0.846 | a |
| gcc | line | throughput | b / galoot | 1.168 | galoot | 1.169 | galoot |
| gcc | line | throughput | c / galoot | 1.097 | - | 1.097 | galoot |
| gcc | line | latency | b / a | 1.076 | - | 1.076 | a |
| gcc | line | latency | bp / a | 1.059 | - | 1.058 | a |
| gcc | line | latency | c / a | 0.929 | - | 0.929 | c |
| gcc | line | latency | c / b | 0.863 | c | 0.864 | c |
| gcc | line | latency | bp / b | 0.984 | - | 0.984 | - |
| gcc | line | latency | a / galoot | 0.830 | a | 0.830 | a |
| gcc | line | latency | b / galoot | 0.893 | b | 0.893 | b |
| gcc | line | latency | c / galoot | 0.771 | c | 0.771 | c |
| gcc | line | batch | b / a | 0.761 | b | 0.761 | b |
| gcc | line | batch | bp / a | 0.760 | bp | 0.760 | bp |
| gcc | line | batch | c / a | 0.810 | c | 0.810 | c |
| gcc | line | batch | c / b | 1.064 | - | 1.064 | b |
| gcc | line | batch | bp / b | 0.998 | - | 0.998 | - |
| gcc | line | batch | a / galoot | 1.207 | galoot | 1.205 | galoot |
| gcc | line | batch | b / galoot | 0.918 | - | 0.917 | b |
| gcc | line | batch | c / galoot | 0.977 | - | 0.976 | - |
| gcc | plane | throughput | b / a | 0.851 | b | 0.849 | b |
| gcc | plane | throughput | bp / a | 0.799 | bp | 0.796 | bp |
| gcc | plane | throughput | c / a | 0.748 | c | 0.746 | c |
| gcc | plane | throughput | c / b | 0.879 | - | 0.879 | c |
| gcc | plane | throughput | bp / b | 0.938 | - | 0.938 | bp |
| gcc | plane | throughput | a / galoot | 1.035 | - | 1.036 | - |
| gcc | plane | throughput | b / galoot | 0.881 | b | 0.879 | b |
| gcc | plane | throughput | c / galoot | 0.774 | c | 0.773 | c |
| gcc | plane | latency | b / a | 0.717 | b | 0.716 | b |
| gcc | plane | latency | bp / a | 0.715 | bp | 0.715 | bp |
| gcc | plane | latency | c / a | 0.838 | c | 0.839 | c |
| gcc | plane | latency | c / b | 1.170 | b | 1.172 | b |
| gcc | plane | latency | bp / b | 0.998 | - | 0.999 | - |
| gcc | plane | latency | a / galoot | 0.749 | a | 0.750 | a |
| gcc | plane | latency | b / galoot | 0.537 | b | 0.537 | b |
| gcc | plane | latency | c / galoot | 0.628 | c | 0.629 | c |
| gcc | plane | batch | b / a | 0.582 | b | 0.582 | b |
| gcc | plane | batch | bp / a | 0.864 | bp | 0.864 | bp |
| gcc | plane | batch | c / a | 0.936 | - | 0.936 | c |
| gcc | plane | batch | c / b | 1.609 | b | 1.608 | b |
| gcc | plane | batch | bp / b | 1.484 | b | 1.483 | b |
| gcc | plane | batch | a / galoot | 1.041 | - | 1.042 | galoot |
| gcc | plane | batch | b / galoot | 0.606 | b | 0.606 | b |
| gcc | plane | batch | c / galoot | 0.975 | - | 0.975 | c |
| clang | point | throughput | b / a | 0.946 | - | 0.952 | b |
| clang | point | throughput | bp / a | 0.935 | - | 0.948 | bp |
| clang | point | throughput | c / a | 0.594 | c | 0.601 | c |
| clang | point | throughput | c / b | 0.628 | c | 0.631 | c |
| clang | point | throughput | bp / b | 0.989 | - | 0.997 | - |
| clang | point | throughput | a / galoot | 0.677 | a | 0.675 | a |
| clang | point | throughput | b / galoot | 0.640 | b | 0.642 | b |
| clang | point | throughput | c / galoot | 0.402 | c | 0.406 | c |
| clang | point | latency | b / a | 0.618 | b | 0.606 | b |
| clang | point | latency | bp / a | 0.560 | bp | 0.551 | bp |
| clang | point | latency | c / a | 0.557 | c | 0.556 | c |
| clang | point | latency | c / b | 0.902 | c | 0.918 | c |
| clang | point | latency | bp / b | 0.906 | - | 0.909 | bp |
| clang | point | latency | a / galoot | 1.057 | - | 1.057 | galoot |
| clang | point | latency | b / galoot | 0.653 | b | 0.641 | b |
| clang | point | latency | c / galoot | 0.588 | c | 0.588 | c |
| clang | point | batch | b / a | 0.324 | b | 0.320 | b |
| clang | point | batch | bp / a | 0.322 | bp | 0.320 | bp |
| clang | point | batch | c / a | 0.537 | c | 0.534 | c |
| clang | point | batch | c / b | 1.659 | b | 1.668 | b |
| clang | point | batch | bp / b | 0.996 | - | 0.998 | - |
| clang | point | batch | a / galoot | 0.334 | a | 0.331 | a |
| clang | point | batch | b / galoot | 0.108 | b | 0.106 | b |
| clang | point | batch | c / galoot | 0.179 | c | 0.177 | c |
| clang | line | throughput | b / a | 1.083 | - | 1.082 | a |
| clang | line | throughput | bp / a | 1.107 | a | 1.104 | a |
| clang | line | throughput | c / a | 0.753 | c | 0.753 | c |
| clang | line | throughput | c / b | 0.695 | c | 0.697 | c |
| clang | line | throughput | bp / b | 1.021 | - | 1.021 | b |
| clang | line | throughput | a / galoot | 0.740 | a | 0.739 | a |
| clang | line | throughput | b / galoot | 0.802 | b | 0.799 | b |
| clang | line | throughput | c / galoot | 0.557 | c | 0.557 | c |
| clang | line | latency | b / a | 0.804 | b | 0.835 | b |
| clang | line | latency | bp / a | 0.796 | bp | 0.824 | bp |
| clang | line | latency | c / a | 0.547 | c | 0.559 | c |
| clang | line | latency | c / b | 0.680 | c | 0.670 | c |
| clang | line | latency | bp / b | 0.989 | - | 0.987 | bp |
| clang | line | latency | a / galoot | 0.793 | a | 0.794 | a |
| clang | line | latency | b / galoot | 0.638 | b | 0.663 | b |
| clang | line | latency | c / galoot | 0.434 | c | 0.444 | c |
| clang | line | batch | b / a | 0.404 | b | 0.399 | b |
| clang | line | batch | bp / a | 0.403 | bp | 0.399 | bp |
| clang | line | batch | c / a | 0.572 | c | 0.566 | c |
| clang | line | batch | c / b | 1.415 | b | 1.418 | b |
| clang | line | batch | bp / b | 0.997 | - | 0.999 | - |
| clang | line | batch | a / galoot | 0.399 | a | 0.404 | a |
| clang | line | batch | b / galoot | 0.161 | b | 0.161 | b |
| clang | line | batch | c / galoot | 0.228 | c | 0.229 | c |
| clang | plane | throughput | b / a | 0.878 | b | 0.877 | b |
| clang | plane | throughput | bp / a | 1.039 | - | 1.039 | a |
| clang | plane | throughput | c / a | 0.735 | c | 0.735 | c |
| clang | plane | throughput | c / b | 0.838 | c | 0.838 | c |
| clang | plane | throughput | bp / b | 1.184 | b | 1.184 | b |
| clang | plane | throughput | a / galoot | 0.412 | a | 0.412 | a |
| clang | plane | throughput | b / galoot | 0.362 | b | 0.362 | b |
| clang | plane | throughput | c / galoot | 0.303 | c | 0.303 | c |
| clang | plane | latency | b / a | 0.750 | b | 0.749 | b |
| clang | plane | latency | bp / a | 0.910 | - | 0.908 | bp |
| clang | plane | latency | c / a | 0.692 | c | 0.691 | c |
| clang | plane | latency | c / b | 0.922 | - | 0.922 | c |
| clang | plane | latency | bp / b | 1.212 | b | 1.212 | b |
| clang | plane | latency | a / galoot | 0.530 | a | 0.530 | a |
| clang | plane | latency | b / galoot | 0.397 | b | 0.397 | b |
| clang | plane | latency | c / galoot | 0.366 | c | 0.366 | c |
| clang | plane | batch | b / a | 0.329 | b | 0.328 | b |
| clang | plane | batch | bp / a | 0.325 | bp | 0.329 | bp |
| clang | plane | batch | c / a | 0.463 | c | 0.478 | c |
| clang | plane | batch | c / b | 1.410 | b | 1.457 | b |
| clang | plane | batch | bp / b | 0.988 | - | 1.002 | - |
| clang | plane | batch | a / galoot | 0.206 | a | 0.205 | a |
| clang | plane | batch | b / galoot | 0.068 | b | 0.067 | b |
| clang | plane | batch | c / galoot | 0.095 | c | 0.098 | c |

### Sweep: cycles per transform against objects per motor (batch loop, size 0, median of 5)

| Compiler | Operation | Form | 1 | 2 | 3 | 4 | 6 | 8 | 16 | 1024 (run) |
| --- | --- | --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| gcc | point | galoot | 50.6 | 50.2 | 50.1 | 50.0 | 49.9 | 49.9 | 49.8 | 49.8 |
| gcc | point | a | 55.7 | 55.7 | 55.5 | 55.6 | 55.5 | 55.5 | 55.5 | 55.4 |
| gcc | point | b | 40.8 | 33.9 | 31.5 | 30.2 | 28.7 | 28.0 | 27.1 | 26.2 |
| gcc | point | c | 34.4 | 33.8 | 33.6 | 32.5 | 31.6 | 31.5 | 31.4 | 31.2 |
| gcc | line | galoot | 57.6 | 57.0 | 56.9 | 56.9 | 56.9 | 56.8 | 56.8 | 56.8 |
| gcc | line | a | 70.0 | 69.3 | 69.1 | 68.9 | 68.7 | 68.7 | 68.6 | 68.4 |
| gcc | line | b | 73.9 | 63.6 | 59.6 | 57.7 | 55.7 | 54.8 | 53.5 | 52.1 |
| gcc | line | c | 63.1 | 59.4 | 58.0 | 57.6 | 56.6 | 56.2 | 55.6 | 55.4 |
| gcc | plane | galoot | 47.0 | 46.6 | 46.5 | 46.4 | 46.3 | 46.3 | 46.3 | 46.2 |
| gcc | plane | a | 49.7 | 49.2 | 48.9 | 48.6 | 48.4 | 48.4 | 48.2 | 48.1 |
| gcc | plane | b | 42.6 | 35.4 | 33.4 | 32.2 | 30.6 | 30.2 | 29.1 | 28.0 |
| gcc | plane | c | 52.3 | 49.4 | 47.2 | 47.0 | 46.5 | 46.1 | 45.6 | 45.0 |
| clang | point | galoot | 50.0 | 49.5 | 49.2 | 49.2 | 49.1 | 49.1 | 49.0 | 49.0 |
| clang | point | a | 33.2 | 21.1 | 26.2 | 19.3 | 19.4 | 18.1 | 17.3 | 16.2 |
| clang | point | b | 33.7 | 22.0 | 19.4 | 15.7 | 14.2 | 11.5 | 8.5 | 5.2 |
| clang | point | c | 21.8 | 14.3 | 16.6 | 12.9 | 14.0 | 11.1 | 10.0 | 8.7 |
| clang | line | galoot | 64.3 | 63.6 | 63.4 | 63.3 | 63.2 | 63.2 | 63.2 | 63.0 |
| clang | line | a | 49.6 | 39.4 | 38.9 | 30.2 | 28.4 | 27.5 | 26.1 | 25.5 |
| clang | line | b | 55.0 | 37.0 | 29.3 | 28.0 | 23.7 | 19.0 | 14.8 | 10.2 |
| clang | line | c | 37.4 | 23.3 | 27.1 | 21.4 | 18.1 | 18.2 | 16.2 | 14.4 |
| clang | plane | galoot | 78.0 | 79.7 | 77.6 | 77.6 | 77.7 | 77.7 | 79.1 | 77.5 |
| clang | plane | a | 38.8 | 21.1 | 26.4 | 19.3 | 19.8 | 17.9 | 17.1 | 15.9 |
| clang | plane | b | 28.8 | 19.7 | 17.1 | 14.2 | 15.5 | 12.4 | 8.8 | 5.2 |
| clang | plane | c | 24.8 | 15.6 | 15.5 | 12.1 | 12.8 | 10.9 | 9.5 | 7.6 |

Break-even: the smallest batch size from which the first form takes fewer cycles per transform (medians) than the second at that size and every larger one, the run's 1,024 included.

| Compiler | Operation | b against a | b against galoot | c against a | b against c |
| --- | --- | --- | --- | --- | --- |
| gcc | point | 1 | 1 | 1 | 3 |
| gcc | line | 2 | 6 | 1 | 6 |
| gcc | plane | 1 | 1 | 3 | 1 |
| clang | point | 3 | 1 | 1 | 16 |
| clang | line | 2 | 1 | 1 | 16 |
| clang | plane | 1 | 1 | 1 | 16 |

