# pintos3

Tests:

pass tests/userprog/halt
-----
pass tests/userprog/exit
-----
pass tests/userprog/exec-once
pass tests/userprog/exec-multiple
pass tests/userprog/exec-arg
pass tests/userprog/exec-missing
pass tests/userprog/exec-bad-ptr
-----
pass tests/userprog/wait-simple
pass tests/userprog/wait-twice
pass tests/userprog/wait-killed
pass tests/userprog/wait-bad-pid
-----
pass tests/userprog/create-empty
pass tests/userprog/create-long
pass tests/userprog/create-normal
pass tests/userprog/create-exists
pass tests/userprog/create-null
pass tests/userprog/create-bad-ptr
pass tests/userprog/create-bound
-----
pass tests/userprog/open-missing
pass tests/userprog/open-normal
pass tests/userprog/open-twice
pass tests/userprog/open-boundary
pass tests/userprog/open-empty
pass tests/userprog/open-null
pass tests/userprog/open-bad-ptr
-----
pass tests/userprog/close-normal
pass tests/userprog/close-twice
pass tests/userprog/close-stdin
pass tests/userprog/close-stdout
pass tests/userprog/close-bad-fd
-----
pass tests/userprog/read-normal
pass tests/userprog/read-zero
pass tests/userprog/read-bad-ptr
pass tests/userprog/read-boundary
pass tests/userprog/read-stdout
pass tests/userprog/read-bad-fd