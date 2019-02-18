import random
import math
 
def get_prime():
    while True:
        r=random.randint(10000000,20000000)
        if is_prime(r):
            return r

def is_prime(n):
    """
    Miller-Rabin primality test.
 
    A return value of False means n is certainly not prime. A return value of
    True means n is very likely a prime.
    """
    if n!=int(n):
        return False
    n=int(n)
    #Miller-Rabin test for prime
    if n==0 or n==1 or n==4 or n==6 or n==8 or n==9:
        return False
 
    if n==2 or n==3 or n==5 or n==7:
        return True
    s = 0
    d = n-1
    while d%2==0:
        d>>=1
        s+=1
    assert(2**s * d == n-1)
 
    def trial_composite(a):
        if pow(a, d, n) == 1:
            return False
        for i in range(s):
            if pow(a, 2**i * d, n) == n-1:
                return False
        return True  
 
    for i in range(8):#number of trials 
        a = random.randrange(2, n)
        if trial_composite(a):
            return False
 
    return True  

def primRoots(p):
    fact=[]
    phi=p-1
    n=phi
    lim=round(math.sqrt(n))
    for i in range(2,lim):
        if n % i == 0:
            fact.append(i)
            while (n % i == 0):
                n /= i
    if n > 1:
        fact.append(n)
 
    for res in range(2,p+1):
        flag=True
        
        for i in range(0,len(fact)):
            if flag:
                flag &=power(res,round(phi/fact[i]),p) != 1
        if flag:
            return res
    
    return -1



def power(x,y,m): 
  
    res = 1
    while (y > 0): 
        if ((y & 1) == 1) : 
            res =( res * x ) % m
 
        y = y >> 1
        x =( x * x ) % m
      
    return res 


def modInverse(a,m) : 
    m0 = m 
    y = 0
    x = 1
    if (m == 1) : 
        return 0
    while (a > 1) : 
        q = a // m 
        t = m 
        m = a % m 
        a = t 
        t = y 
        y = x - q * y 
        x = t 

    if (x < 0) : 
        x = x + m0 
  
    return x 

#print(is_prime(32416190071))
#a=get_prime()
#print(a)
#print(primRoots(a))