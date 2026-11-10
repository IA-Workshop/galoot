### probe.clang.no-slp.s

| operation | mul lanes: computed, live, padding | add/sub lanes: computed, live, padding | neg: computed, live | shared live products | results equal the reference | monomials per result |
| --- | --- | --- | --- | ---: | --- | --- |
| wedge_point_point | 12, 12, 0 | 6, 6, 0 | 6, 4 | 0 | yes | [2, 2, 2, 2, 2, 2] |
| wedge_point_point_point | 24, 24, 0 | 14, 14, 0 | 4, 2 | 0 | yes | [6, 6, 6, 6] |
| antiwedge_line_plane | 12, 12, 0 | 8, 8, 0 | 1, 1 | 0 | yes | [3, 3, 3, 3] |
| transform_point | 48, 48, 0 | 36, 36, 0 | 1, 1 | 0 | yes | [12, 12, 12, 4] |
| transform_line | 72, 72, 0 | 58, 58, 0 | 6, 4 | 0 | yes | [8, 8, 8, 20, 20, 20] |
| transform_plane | 48, 48, 0 | 36, 36, 0 | 3, 2 | 0 | yes | [8, 8, 8, 16] |
| compose_motors | 48, 48, 0 | 40, 40, 0 | 9, 7 | 0 | yes | [4, 4, 4, 4, 8, 8, 8, 8] |

- wedge_point_point: lanes no stored result reads: 2 xorpd
- wedge_point_point_point: lanes no stored result reads: 2 xorpd
- transform_line: lanes no stored result reads: 2 xorpd
- transform_plane: lanes no stored result reads: 1 xorpd
- compose_motors: lanes no stored result reads: 2 xorpd

### probe.clang.no-copyable.s

| operation | mul lanes: computed, live, padding | add/sub lanes: computed, live, padding | neg: computed, live | shared live products | results equal the reference | monomials per result |
| --- | --- | --- | --- | ---: | --- | --- |
| wedge_point_point | 12, 12, 0 | 6, 6, 0 | 6, 4 | 0 | yes | [2, 2, 2, 2, 2, 2] |
| wedge_point_point_point | 24, 24, 0 | 14, 14, 0 | 4, 2 | 0 | yes | [6, 6, 6, 6] |
| antiwedge_line_plane | 12, 12, 0 | 8, 8, 0 | 1, 1 | 0 | yes | [3, 3, 3, 3] |
| transform_point | 48, 48, 0 | 36, 36, 0 | 1, 1 | 0 | yes | [12, 12, 12, 4] |
| transform_line | 72, 72, 0 | 60, 58, 0 | 7, 4 | 0 | yes | [8, 8, 8, 20, 20, 20] |
| transform_plane | 48, 48, 0 | 42, 36, 0 | 4, 2 | 0 | yes | [8, 8, 8, 16] |
| compose_motors | 48, 48, 0 | 40, 40, 0 | 9, 7 | 0 | yes | [4, 4, 4, 4, 8, 8, 8, 8] |

- wedge_point_point: lanes no stored result reads: 2 xorpd
- wedge_point_point_point: lanes no stored result reads: 2 xorpd
- transform_line: lanes no stored result reads: 1 addpd, 1 subpd, 3 xorpd
- transform_plane: lanes no stored result reads: 3 addpd, 3 subpd, 2 xorpd
- compose_motors: lanes no stored result reads: 2 xorpd

### probe.clang.inline-2000.s

| operation | mul lanes: computed, live, padding | add/sub lanes: computed, live, padding | neg: computed, live | shared live products | results equal the reference | monomials per result |
| --- | --- | --- | --- | ---: | --- | --- |
| wedge_point_point | 12, 12, 0 | 6, 6, 0 | 6, 4 | 0 | yes | [2, 2, 2, 2, 2, 2] |
| wedge_point_point_point | 24, 24, 0 | 14, 14, 0 | 4, 2 | 0 | yes | [6, 6, 6, 6] |
| antiwedge_line_plane | 12, 12, 0 | 8, 8, 0 | 1, 1 | 0 | yes | [3, 3, 3, 3] |
| transform_point | 48, 48, 0 | 36, 36, 0 | 1, 1 | 0 | yes | [12, 12, 12, 4] |
| transform_line | 72, 72, 0 | 58, 58, 0 | 7, 4 | 0 | yes | [8, 8, 8, 20, 20, 20] |
| transform_plane | 48, 48, 0 | 42, 36, 0 | 4, 2 | 0 | yes | [8, 8, 8, 16] |
| compose_motors | 48, 48, 0 | 40, 40, 0 | 12, 7 | 0 | yes | [4, 4, 4, 4, 8, 8, 8, 8] |

- wedge_point_point: lanes no stored result reads: 2 xorpd
- wedge_point_point_point: lanes no stored result reads: 2 xorpd
- transform_line: lanes no stored result reads: 3 xorpd
- transform_plane: lanes no stored result reads: 3 addpd, 3 subpd, 2 xorpd
- compose_motors: lanes no stored result reads: 5 xorpd

