# Two functions whose results hold no blade, so that each compiles to a
# return alone and names no register.
	.text
_Z5firstRKd:
	ret
_Z6secondv:                             # @_Z6secondv
	retq
