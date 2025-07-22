[TOC]

# Lecture 01: Course Overview

**Perspective projection**: 

- Objects look smaller as they get further away ("perspective")
- Why does this happen?
  - As the the `z` becomes bigger and bigger, the `v` becomes smaller and smaller. That's why this happens.
- Consider simple ("pinhole") model of a camera:
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107165844724.png" alt="image-20250107165844724" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107170637610.png" alt="image-20250107170637610" style="zoom:25%;" />

---

> How do we depict a line on computer?

**Raster display:**

- Image represented as a 2D grid of "pixels" (picture elements)
- Each pixel can take on a unique color value

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107183450018.png" alt="image-20250107183450018" style="zoom:25%;" />



> [!NOTE]
>
> We will strongly challenge this notion of a pixel "as a little square" soon enough. But let's go with it not ;-)

**Rasterization:** process of converting a continuous object to a discrete representation on a raster grid (pixel grid)

---

> What pixels should we color to depict a line?

**Diamond rule (used by modern GPUs):** light up pixel if line pass through associated diamond

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107184126523.png" alt="image-20250107184126523" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107184200227.png" alt="image-20250107184200227" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107184328161.png" alt="image-20250107184328161" style="zoom:25%;" />

---

> How do we find the pixels satisfying a chosen rasterization rule?

**Incremental line rasterization**

- Let's say a line is represented with integer endpoints:  $ (u_1, v_1)$

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107185838362.png" alt="image-20250107185838362" style="zoom:35%;" />

- Slope of line: $s=(v_2-v_1)/(u_2-u_1)$ 
- Consider an easy special case:
  - $u_1<u_2$, $v_1<v_2$ (line points toward upper-right)
  - $0<s<1$ (more change in $x$ than $y$)

The pseudo algorithm

```c
v = v1
for (u=u1; u<=u2; u++) {
    v += s;
    draw(u, round(v))
}
```

> [!NOTE]
>
> Actually is slope is bigger than 1, it will change. And this is not how modern method does.

# Lecture 02: Linear Algebra (P)Review

> How do we measure vectors?

**Vector**: direction and magnitude

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107200228641.png" alt="image-20250107200228641" style="zoom:25%;" />

**Cartesian coordinates:** use coordinate system to measure vector

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107200600034.png" alt="image-20250107200600034" style="zoom:33%;" />

> [!WARNING]
>
> Can't directly compare coordinates in different systems! Also shouldn't compare $(r,\theta)$ to $(x,y)$

---

> What  can we do with a vector?

- "End to end": addition (communicative)
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107201219421.png" alt="image-20250107201219421" style="zoom:33%;" />

- Scaling
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250107201355382.png" alt="image-20250107201355382" style="zoom:33%;" />

---

> Can function work as vectors?

- We can certainly add two function:
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113134626327.png" alt="image-20250113134626327" style="zoom:33%;" />
- We can also scale a function:
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113134643021.png" alt="image-20250113134643021" style="zoom:33%;" />

And we can also do other operations works for vectors on functions!

Short answer: **Yes, functions are vectors!** (Even if they don't look like "little arrows".)

---

> How can we measure a Vector?

**Length**—Norm of a Vector:

- Let's start with magnitude—for a given vector $v$, we want to assign it a number $|v|$ called its **length** or **magnitude** or **norm**.

- Intuitively, the norm should capture how "big" the vector is.

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113135822341.png" alt="image-20250113135822341" style="zoom:44%;" />

- Norm of a function example: $L^2$ norm of function $\|f\|:=\sqrt{\int_0^1f(x)^2dx}$

> [!NOTE]
>
> For clarity we will use $|·|$ for the norm of a  vector in $\mathbb{R}^n$, and $\|·\|$ for the norm of a function.

**Orientation**—Inner Product (scalar product or dot product):

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113141710079.png" alt="image-20250113141710079" style="zoom:33%;" />

- Symmetry: $\langle u,v\rangle=\langle v,u\rangle$
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113141835540.png" alt="image-20250113141835540" style="zoom:33%;" />

- $L^2$ Inner Product of Functions—measure how well two functions "line up", e.g., for square-integrable functions on the unit interval: $\langle\!\langle f, g \rangle\!\rangle := \int_{0}^{1} f(x) g(x) \, dx$

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113143340248.png" alt="image-20250113143340248" style="zoom:50%;" />

> [!IMPORTANT]
>
> - Many ways to measure "how big" a signal is (norm) or "how well-aligned" two signals are (inner product).
> - Choice of inner product depends on application. 

---

**Linear Maps**: said linear algebra was study of **vector spaces** and **linear maps** between them.

> Why linear maps is useful for graphics?

- Computationally, easy to solve systems of linear equations.

- Basic transformations (rotation, translation, scaling) can be expressed as linear maps.

- *All* maps can be approximated as linear maps over a short distance/short time. (Taylor's theorem). This approximation is used all over geometry, animation, rendering, image processing…

  > **短距离/短时间：复杂变换看起来是直线的**
  > 想象你在地球表面上行走：从全球视角来看，地球是一个球体（非线性表面）；但如果只看你脚下的小区域，地球看起来是平的，这就是局部线性化的例子。

---

Linear Maps—**Algebraic Definition**, Key idea: linear maps take lines to lines

- A map $f$ is linear if it maps vectors to vectors, and if for all vector $u,v$ and scalars $a$ we have
  $$
  f(u+v)=f(u)+f(v)\\
  f(au)=af(u)
  $$

- Order of linear maps doesn't matter:
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113145635556.png" alt="image-20250113145635556" style="zoom:40%;" />

> [!NOTE]
>
> $f(x)=ax+b$ is not a linear map function, it is called an AFFINE function

---

**Connection** between "span" and "linear map"

*"The <u>image</u> of any linear map is the span of some collection of vectors."*

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113150953027.png" alt="image-20250113150953027" style="zoom:50%;" />

---

> Given a collection of basis vectors $a_1, … a_n$, how do we find an **orthonormal** basis $e_1, ... e_n$?

**Gram-Schmidt** algorithm:

1. normalize the first vector (i.e., divide by its length)
2. subtract any component of the 1st vector from the 2nd one
3. normalize the 2nd one
4. repeat, removing components of first k vectors from vector k+1

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113152009178.png" alt="image-20250113152009178" style="zoom:50%;" />

> [!WARNING]
>
> For large number of vectors / nearly parallel vectors, not the best algorithm…

---

**Fourier Transform**

> Functions are also vectors. Do they have an orthonormal basis?

Yes! This is the basic idea behind the *Fourier transform*.

Simple example: functions that repeat at intervals of $2\pi$:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113152448416.png" alt="image-20250113152448416" style="zoom:50%;" />

**Can project onto basis of sinusoids**: $cos(nx), sin(mx), m,n\in\mathbb{N}$

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113152708780.png" alt="image-20250113152708780" style="zoom:50%;" />

- really just a linear map from on basis to another
- fundamental building block many graphics algorithm

# Lecture 03: Vector Calculus (P)Review

**Cross Product**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113163600769.png" alt="image-20250113163600769" style="zoom:50%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113163415051.png" alt="image-20250113163415051" style="zoom:50%;" />

- Inner product takes two vectors and produces a scalar

- In 3D, cross product is a natural way to take two vectors and get a vector, written as $u\times v$

- Geometrically:

  - magnitude equal to parallelogram area
  - direction orthogonal to both vectors

- $\sqrt{det(u,v,u\times v)}=|u||v|sin(\theta)$

- $$
  u \times v:=
  \left[\begin{array}{ll}
  u_2v_3-u_3v_2\\
  u_3v_1-u_1v_3\\
  u_1v_2-u_2v_1
  \end{array}
  \right]
  $$

---

**Cross Product as Quarter Rotation**

- Simple but useful observation for manipulation vector in 3D: cross product with a unit vector $n$ is equivalent to a quarter-rotation in the plane with normal $n$
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113164831992.png" alt="image-20250113164831992" style="zoom:50%;" />

---

**Matrix Representation of Inner/Cross Product/Linear Maps**

- Inner Product: e.g. $\langle u,v\rangle:=2u_1v_1+u_1v_2+u_2v_1+3u_2v_2$
  $$
  \langle u \times v \rangle=u^TAv=
  \left[\begin{array}{ll}
  u_1 & u_2
  \end{array}
  \right]
  \left[\begin{array}{ll}
  2 & 1\\
  1 & 3
  \end{array}
  \right]
  \left[\begin{array}{ll}
  v_1\\
  v_2
  \end{array}
  \right]
  $$

- Cross Product: e.g. 
  $$
  \hat{u}:=
  
  \left[\begin{array}{ll}
  0&-u_3&u_2\\
  u_3&0&-u_1\\
  -u_2&u_1&0
  \end{array}
  \right]\\
  
  u \times v=\hat{u}v=
  \left[\begin{array}{ll}
  0&-u_3&u_2\\
  u_3&0&-u_1\\
  -u_2&u_1&0
  \end{array}
  \right]
  \left[\begin{array}{ll}
  v_1\\
  v_2\\
  v_3
  \end{array}
  \right]
  \\
  v \times u=-\hat{u}v=\hat{u}^Tv
  $$

- Linear Maps: e.g.
  $$
  f(u)=u_1a_1+u_2a_2+u_3a_3 \\
  A:=
  \begin{bmatrix}
  \mathbf{a}_1 & \mathbf{a}_2 & \mathbf{a}_3
  \end{bmatrix}
  \\
  f(u):=A
  \left[\begin{array}{ll}
  v_1\\
  v_2\\
  v_3
  \end{array}
  \right]
  $$
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113193207962.png" alt="image-20250113193207962" style="zoom:50%;" />

---

**Understand Determinant from Cross Product**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113171035608.png" alt="image-20250113171035608" style="zoom:50%;" />

Known as a "triple product formula"

---

**Determinant of a Linear Map**

> If a matrix A encodes a linear map $f$, what does $det(A)$ means?

- It measures the change in volume

> What does the sign of the determinant tell us, in this case?

- It tells us whether orientation was reversed ($det(A)<0$)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113193424398.png" alt="image-20250113193424398" style="zoom:50%;" />

---

**Differential Operators**

> Why is this useful for computer graphics?

- Many physical/geometric problems expressed in terms of relative rates of change.
- These tools also provide foundation for numerical optimization—e.g., minimize cost by following the gradient of some objective.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113194521391.png" alt="image-20250113194521391" style="zoom:50%;" />

---

**Derivative as Slope**
$$
f'(x_0):=\lim_{\epsilon\to 0}\frac{f(x_0+\epsilon)-f(x_0)}{\epsilon}
$$


<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113194557563.png" alt="image-20250113194557563" style="zoom:50%;" />

> [!CAUTION]
>
> What if slope is different when we walk in opposite direction?
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113195034515.png" alt="image-20250113195034515" style="zoom:50%;" />
> $$
> f^+(x_0):=\lim_{\epsilon\to 0}\frac{f(x_0+\epsilon)-f(x_0)}{\epsilon}\\
> f^-(x_0):=\lim_{\epsilon\to 0}\frac{f(x_0)-f(x_0-\epsilon)}{\epsilon}
> $$
> 

---

**Derivative as Best Linear Approximation**

- Any **smooth** function $f(x)$ can be expressed as a *Taylor series*:
  $$
  f(x)=f(x_0)+f'(x_0)(x-x_0)+\frac{(x-x_0)^2}{2!}f''(x_0)+···
  $$
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113200116050.png" alt="image-20250113200116050" style="zoom:50%;" />

- Replacing complicated functions with a linear (and sometimes quadratic) approximation is a powerful trick in graphic algorithms—we'll see many examples

---

**Directional Derivative**

> How do we think about derivatives for a function that has multiple variables?

- One way: suppose we have a function $f(x_1,x_2)$

  - Take a "slice" through the function alone some line
  - Then just apply the usual derivative!
  - Called the $\text{\color{blue}{directional derivative}}$.

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113201108918.png" alt="image-20250113201108918" style="zoom:50%;" />
  $$
  D_uf(\textbf{x}_0):=\lim_{\epsilon\to 0}\frac{f(\textbf{x}_0+\epsilon u)-f(\textbf{x}_0)}{\epsilon}\\
  \text{\color{red}{take a small step along }u}
  $$

---

**Gradient**

- Given a multivariable function $f(\textbf{x})$, $\text{\color{blue}{gradient }}\nabla f(\textbf{x})$ assigns a vector at each point

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250113201516645.png" alt="image-20250113201516645" style="zoom:50%;" />
  $$
  \nabla f=
  \begin{bmatrix}
  \partial f /\partial x_1\\
  \vdots\\
  \partial f /\partial x_n\\
  \end{bmatrix}
  $$

> [!NOTE]
>
> Two potential problems:
>
> - Role of inner product is not clear (more later!)
> - <u>No way to differentiate function of functions $F(f)$</u> since we don't have a finite list of coordinated $x_1, …, x_n$

---

**Gradient as Best Linear Approximation**

Another way to think about it: at each point $x_0$, gradient is the vector $\nabla f(\textbf{x}_0)$ that leads to the best possible approximation:
$$
f(\textbf{x})≈f(\textbf{x}_0)+\langle\nabla f(\textbf{x}_0),\textbf{x}-\textbf{x}_0\rangle
$$
<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114201756896.png" alt="image-20250114201756896" style="zoom:50%;" />

> [!TIP]
>
> - Another way to think about it: direction of "steepest ascent" / "the gradient takes you uphill"
> - I.e., what the direction should we travel to increase value of function as quickly as possible
> - This viewpoint leads to algorithms for optimization, commonly used in graphics.
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114202109711.png" alt="image-20250114202109711" style="zoom:50%;" />

---

**Gradient and Directional Derivative**

At each point $x$, gradient is unique vector $\nabla f(\textbf{x})$ such that $\langle \nabla f(\textbf{x}),\textbf{u} \rangle=D_\textbf{u}f(\textbf{x})$

> [!WARNING]
>
> $\text{\color{red}{Can't happen if function is not differentiable!}}$
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114202538994.png" alt="image-20250114202538994" style="zoom:50%;" />

---

**Gradients of Matrix-Valued Expressions**

- $\text{\color{red}{EXTREMELY}}$ useful in graphics to be able to differentiate expressions involving matrices
- Ultimately, expressions look much like ordinary derivatives

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114203530326.png" alt="image-20250114203530326" style="zoom:50%;" />

> It is just too pain in the butt to type the table in latex format, so I give up :) 

---

**Vector Fields**

- Gradient was our first example of a $\text{\color{blue}{vector field}}$
- In general, a vector field assigns a vector to each point in space
- E.g., can think of a 2-vector field in the plane as a map: $X:\mathbb{R}^2\to\mathbb{R}^2$, like $\nabla f(x,y)=(2x,2y)$

---

> How do we measure change in a vector field?
>
> How much is field shrinking/expanding?
>
> How much is the field spinning?
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114210718329.png" alt="image-20250114210718329" style="zoom:50%;" />

**Divergence**

> 散度衡量的是一个向量场在某一点的“扩散”程度。你可以把它理解为在一个点周围向外或向内流动的强度。
>
> - 如果散度为正（>0），说明该点的流量是向外扩散的，类似于一个源点。
> - 如果散度为负（<0），则表示流量是向内汇聚的，类似于一个汇点。
> - 如果散度为零，则说明没有净流出或流入，向量场在该点处保持平衡。

- Also commonly written as $\nabla\vdot X$

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114211652169.png" alt="image-20250114211652169" style="zoom:50%;" />

- Think $\nabla$ as a "vector of derivatives"
  $$
  \nabla=(\frac{\partial}{\partial u_1},...,\frac{\partial}{\partial u_n})
  $$

- Think of $X$ as a "vector of functions"
  $$
  X(\textbf{u})=(X_1(\textbf{u}),...,X_n(\textbf{u}))
  $$

- Then **divergence** is
  $$
  \partial \vdot X := \sum_{i=1}^n\partial X_i/\partial u_i
  $$

Example:

- Consider the vector field $X(u,v):=(cos(u),sin(v))$

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114212437011.png" alt="image-20250114212437011" style="zoom:50%;" />

- Divergence is then: $\nabla \vdot X=\frac{\partial}{\partial u}cos(u)+\frac{\partial}{\partial v}sin(u)=-sin(u)+cos(v)$

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114212702478.png" alt="image-20250114212702478" style="zoom:50%;" />



---

**Curl**

> 旋度衡量的是向量场的“旋转”程度。你可以把它理解为在某一点，周围的向量场是否围绕某个轴旋转。具体来说：
>
> - 旋度为零时，说明该点附近没有旋转的趋势。
> - 如果旋度非零，说明存在一定的旋转效应。

- Also commonly written as $\nabla\times X$

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114212828670.png" alt="image-20250114212828670" style="zoom:50%;" />

- This time, think $\nabla$ as a vector of just three derivatives
  $$
  \nabla=(\frac{\partial}{\partial u_1},\frac{\partial}{\partial u_2},\frac{\partial}{\partial u_3})
  $$

- Think of $X$ as vector of three functions
  $$
  X(\textbf{u})=(X_1(\textbf{u}),X_2(\textbf{u}),X_3(\textbf{u}))
  $$

- Then curl is
  $$
  \nabla \times X :=
  
  \left[\begin{array}{ll}
  \partial X_3/\partial u_2 - \partial X_2/\partial u_3\\
  \partial X_1/\partial u_3 - \partial X_3/\partial u_1\\
  \partial X_2/\partial u_1 - \partial X_1/\partial u_2
  \end{array}
  \right]
  $$
  

Example: 

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114213919813.png" alt="image-20250114213919813" style="zoom:50%;" />

> Tired of typing…

Divergence of $X$ is the same as curl of 90-degree rotation of $X$:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114214205376.png" alt="image-20250114214205376" style="zoom:50%;" />

---

**Laplacian—Visual Intuition**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114220813717.png" alt="image-20250114220813717" style="zoom:50%;" />

> For ordinary function $f(x)$, what does 2nd derivative tell us?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114221032588.png" alt="image-20250114221032588" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114221135244.png" alt="image-20250114221135244" style="zoom:50%;" />

---

**Laplacian—Many definitions**

> Laplacian 是一种二阶微分标量算子，广泛应用于数学、物理以及工程学领域，尤其是在描述变化、扩散和传播等现象时。

- Maps a scalar function to another scalar function (*linearly!*)

- Usually* denoted by $\Delta \text{ or } \nabla^2$

- Many starting points for Laplacian:

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114221917775.png" alt="image-20250114221917775" style="zoom:50%;" />

Example:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114222053381.png" alt="image-20250114222053381" style="zoom:50%;" />

---

**Hessian**

- Our final differential operator—$\text{\color{blue}{Hessian}}$ will help us locally approximate complicated functions by a few simple terms
- Recall our *Taylor series*
  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114222403773.png" alt="image-20250114222403773" style="zoom:50%;" />

> How do we do this for multivariable functions?

- Already talked about best *linear* approximation, using gradient:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114222517301.png" alt="image-20250114222517301" style="zoom:50%;" />
$$
\text{\textbf{\color{red}{Hessian gives us more ``quadrati'' term}}}
$$
<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114222740177.png" alt="image-20250114222740177" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250114222901207.png" alt="image-20250114222901207" style="zoom:50%;" />

# Rasterization

# Lecture 04: Drawing a Triangle

**Rasterization**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115135222242.png" alt="image-20250115135222242" style="zoom:50%;" />

- For each primitive (e.g., triangle), which pixels light up?
- Extremely fast (BILLIONS of triangles per second on GPU)
- harder (but not impossible) to achieve photorealism
- perfect match for 2D vector art, fonts, quick 3D preview,…

---

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115135936502.png" alt="image-20250115135936502" style="zoom:50%;" />

Rasterization pipeline converts <u>all</u> primitives to triangles, even points and lines!

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115135951688.png" alt="image-20250115135951688" style="zoom:50%;" />

>  **Why triangles?**

- Triangles can approximate any shape, and
- always planar, well-defined normal (法线), and
- easy to interpolate

Key reason: once everything is reduced to triangles, can focus on making an extremely well-optimized <u>pipeline</u> for drawing` them.

---

**The Rasterization Pipeline**

Rough sketch of rasterization pipeline:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115140653362.png" alt="image-20250115140653362" style="zoom:50%;" />

---

$$
\text{\color{red}{Let's draw some triangles on the screen!}}
$$

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115144911748.png" alt="image-20250115144911748" style="zoom:33%;" />

- Q1: What pixels does the triangle overlap? ("coverage")
  - **COVERAGE**: What scene geometry is hit by a ray from a pixel through the pinhole?
- Q2: Which triangles is closet to the camera in each pixel? ("occlusion")
  - **OCCLUSION**: Which object is the <u>first</u> hit along that ray? 

---

**Computing triangle coverage**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115145222912.png" alt="image-20250115145222912" style="zoom:50%;" />

> What does it mean for a pixel to be covered by a triangle?

Q: Which triangles "cover" this pixel?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115145323617.png" alt="image-20250115145323617" style="zoom:33%;" />

One option: compute the **fraction** of pixel area covered triangle, then color pixel according to this fraction.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115145544291.png" alt="image-20250115145544291" style="zoom:50%;" />

Not very OK when there are multiple triangles. Computing exact coverage **is not practical**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115145650939.png" alt="image-20250115145650939" style="zoom:50%;" />

**Instead**, view coverage as a **sampling** problem

- Do not compute exact/analytical answer
- Instead, test a collection of sample points
- with enough points & smart choice of sample locations, can start to get a good estimate

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115145910937.png" alt="image-20250115145910937" style="zoom:50%;" />

---

**Reconstruction**: given a set of samples, how might we attempt to reconstruct the original signal $f(x)$?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115150319268.png" alt="image-20250115150319268" style="zoom:50%;" />

| Piecewise constant approximation                             | Piecewise linear approximation                               |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115150500517.png" alt="image-20250115150500517" style="zoom:33%;" /> | <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115150539919.png" alt="image-20250115150539919" style="zoom:33%;" /> |

More sampling & reconstruction examples:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115150930187.png" alt="image-20250115150930187" style="zoom:50%;" />

---

**Sampling & Reconstruction**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115152813074.png" alt="image-20250115152813074" style="zoom:50%;" />
$$
\text{\color{red}{Goal: reproduce original signal as accuartely as possible!}}
$$
1D signal can be expressed as a superposition of frequencies

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115153035162.png" alt="image-20250115153035162" style="zoom:50%;" />

---

**Undersampling** high-frequency signals results in <u>**aliasing**</u>: high frequencies in the original signal masquerade as low frequencies after reconstruction (due to undersampling) 

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115153515102.png" alt="image-20250115153515102" style="zoom:50%;" />

Image can also be decomposed into "frequencies":

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115153909429.png" alt="image-20250115153909429" style="zoom:50%;" />

> This slide is very inspiring: as $x$ and $y$ goes up, the sum the square goes up faster, make the undersampling of the periodic funtions $sin(\vdot)$
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115154510294.png" alt="image-20250115154510294" style="zoom:50%;" />

---

**Nyquist-Shannon theorem**

- Consider a **band-limited** signal: has no frequencies above some threshold $\omega_0$

  - 1D example: low-pass filtered audio signal
  - 2D example: blurred image example mentioned above

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115155427513.png" alt="image-20250115155427513" style="zoom:50%;" />

- The signal can be perfectly reconstructed if sampled with period $T=1/2\omega_0$ and if interpolation is performed using a *"since filter"*: ideal filer with no frequencies above cutoff (infinite extent!)

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115155625237.png" alt="image-20250115155625237" style="zoom:50%;" />

---

**Challenges of sampling in computer graphics**

- Signals are often not band-limited in computer graphics.

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115160558606.png" alt="image-20250115160558606" style="zoom:50%;" />

- Also, infinite extent of "ideal" reconstruction filter ($sinc$) is impractical for efficient implementations.

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115160706006.png" alt="image-20250115160706006" style="zoom:50%;" />

---

**Point-in-triangle test**

> How do we check if a given point $q$ is inside a triangle?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115162604848.png" alt="image-20250115162604848" style="zoom:50%;" />

Check if it's contained in three <u>half planes</u> associated with the edges.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115162754304.png" alt="image-20250115162754304" style="zoom:50%;" />

> [!TIP]
>
> Point by point can be wasteful of time: tiled triangle traversal
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115164010510.png" alt="image-20250115164010510" style="zoom:50%;" />
> $$
> \text{\color{red}{This how real graphics hardware works!}}
> $$
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115164321594.png" alt="image-20250115164321594" style="zoom:50%;" />

---

**Summary**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250115164631792.png" alt="image-20250115164631792" style="zoom:50%;" />

# Lecture 05: Spatial Transformation

$$
f:\mathbb{R}^n\to\mathbb{R}^n
$$

In this lecture, we'll focus on common transformation of space (rotation, scaling, etc.) encoded by <u>linear</u> map.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117144609686.png" alt="image-20250117144609686" style="zoom:50%;" />

---

> What does it mean for a map $f:\mathbb{R}^n\to\mathbb{R}^n$ to be <u>linear</u>?

- $\text{\color{blue}{Geometrically:}}$ it maps <u>lines</u> to <u>lines</u>. and preserves the origin

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117145014338.png" alt="image-20250117145014338" style="zoom:50%;" />

- $\text{\color{blue}{Algebraically:}}$ preserves vectors space operations (addition & scaling)

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117144927561.png" alt="image-20250117144927561" style="zoom:50%;" />

---

 $\text{\color{blue}{Composition of linear transformations is linear}}$

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117145214962.png" alt="image-20250117145214962" style="zoom:50%;" />

---

**Types of Transformations:**

> [!NOTE]
>
> In the original video, some pictures are in GIF format. Unfortunately, I cannot represent it in Markdown :(

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117145433623.png" alt="image-20250117145433623" style="zoom:50%;" />

---

**Rotation**:

- keeps origin fixed
- preserve distance
- preserves orientation

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117145655732.png" alt="image-20250117145655732" style="zoom:50%;" />

**2D Rotation**:

*Counter-clockwise is positive
$$
f_\theta=
\left[\begin{array}{ll}
\cos{\theta} & -\sin{\theta}\\
\sin{\theta} & \cos{\theta}
\end{array}
\right]
\left[
\begin{array}{ll}
x_1\\
x_2
\end{array}
\right]
$$
**3D Rotation**:

Just apply the same transformation of two axis and keep the last one fixed

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117150925232.png" alt="image-20250117150925232" style="zoom:50%;" />

**Transpose as Inverse**

Rotation will map standard basis to **new orthonormal** basis $e_1, e_2,e_3$:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117151241429.png" alt="image-20250117151241429" style="zoom:50%;" />

> [!TIP]
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117151421870.png" alt="image-20250117151421870" style="zoom:40%;" />

> [!warning]
>
> Not every matrix $Q^TQ=I$ describes a rotation, for example:
> $$
> Q=\left[
> \begin{array}{cc}
>  -1 & 0\\
>  0 & 1
> \end{array}
> \right]
> \space\space
> Q^TQ=
> \left[
> \begin{array}{cc}
>  (-1)^2 & 0\\
>  0 & 1
> \end{array}
> \right]
> =I
> $$
> But $Q$ will not preserve the <u>orientation</u>
>
> - Rotation additionally <u>preserve</u> orientation: $det(Q) \gt 0$
> - Reflections <u>reverse</u> orientation: $det(Q)\lt0$
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117152226149.png" alt="image-20250117152226149" style="zoom:67%;" />

---

**Scaling**
$$
f(\textbf{u}) = a\textbf{u}, a\in\mathbb{R}\\
\frac{\textbf{u}}{|\textbf{u}|}=\frac{a\textbf{u}}{|a\textbf{u}|}
$$

- Each vector gets mapped to a scalar umltiple
- Preserves the **direction** of all vectors

Matrix representation (when $a$ is negative, this is reflection operation instead of scaling):

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117152733206.png" alt="image-20250117152733206" style="zoom:50%;" />

Scale along any direction: $A:=R^TDR$, where $R$ is rotation operation. First rotate the desired direction to an axis, then scale, the rotate back.

> [!TIP]
>
> $\text{\color{blue}{Every symmetric matrix performs a non-uniform scaling along some set of orthogonal axes.}}$
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117153736624.png" alt="image-20250117153736624" style="zoom:50%;" />

---

**Shear**

A shear displaces each point $\textbf{x}$ in a direction $\textbf{u}$ according to its distance along a fixed vector $\textbf{v}$:
$$
f_{\textbf{u,v}}(\textbf{x})=\textbf{x}+\langle \textbf{v,x}\rangle\textbf{u}
$$
 <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117163708850.png" alt="image-20250117163708850" style="zoom:50%;" />

> Till now, I don't get it actually :(

---

**Composite Transformations**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117164214140.png" alt="image-20250117164214140" style="zoom:50%;" />

---

**Decomposition of Linear Transformations**

**No unique way to do this!** However, there are many useful decompositions:

- Singular value decomposition (good for signal processing) 
- LU factorization (good for solving linear systems)
- polar decomposition (good for spatial transformations) 
- …

Example:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117164541754.png" alt="image-20250117164541754" style="zoom:50%;" />

**Polar decomposition** decomposes any matrix $A$ into orthogonal matrix $Q$ and symmetric positive-semidefinite matrix $P$.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117165229604.png" alt="image-20250117165229604" style="zoom:50%;" />

Since $P$ is symmetric, can take this further via the spectral decomposition $P=VDV^T$ ($V$ orthogonal, $D$ diagonal):

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117165440501.png" alt="image-20250117165440501" style="zoom:50%;" />

Result $UDV^T$ is called the <u>**singular value decomposition**</u>

---

**Interpolating Transformations**

> How are these decompositions useful for graphics?

Consider interpolating between two linear transformations $A_0,A_1$ of some initial model

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117165944985.png" alt="image-20250117165944985" style="zoom:50%;" />
$$
\text{\color{blue}{Goal: animate transition with some nice continuous motion}}
$$
Try **Linear**:
$$
A(t)=(1-t)A_0+tA_1
$$

$$
\text{\color{red}{Hits the right start/endpoints… but looks awful in between!}}
$$

> Here I **highly** recommend to watch the original video, it is funny XD

Try **Polar**:
$$
A_0=Q_0P_0, A_1=Q_1P_1
$$
<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117184231103.png" alt="image-20250117184231103" style="zoom:50%;" />

---

**Translation**

> So far we've ignored a basic transformation—translations

$$
f_\textbf{u}(\textbf{x})=\textbf{x}+\textbf{u}
$$

> [!warning]
>
> Translations transformation is not linear!
> $$
> f_\textbf{u}(\textbf{x})+f_\textbf{u}(\textbf{y})\ne f_\textbf{u}(\textbf{x+y})
> $$
> Translation is $\text{\color{red}{affine, not linear}}$

---

**Homogeneous coordinates**

> [!TIP]
>
> Maybe translations turn into <u>linear</u> transformations if we go into 4th dimension…!

- Consider any 2D plane that does not pass through the origin $\textbf{o}$ in 3D

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117185537351.png" alt="image-20250117185537351" style="zoom:50%;" />

- Every line through the origin in 3D corresponds toa point in the 2D plane

  Just find the point $\textbf{p}$ where the line $L$ pierces the plane

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117185707564.png" alt="image-20250117185707564" style="zoom:50%;" />

- <u>Hence</u>, **any point $\hat{\textbf{p}}$ on the line $L$ can be used to represent the point $\textbf{p}$**

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117185815831.png" alt="image-20250117185815831" style="zoom:50%;" />

- More explicitly, consider a point $\textbf{p}=(x,y)$, and the plane $z=1$ in 3D. Any three numbers $\hat{\textbf{p}}=(a,b,c)$ such that $(a/c,b/c)=(x,y)$ are <u>**homogeneous coordinates**</u> for $\textbf{p}$.

  - E.g., $(x,y,1)$
  - In general: $(cx,cy,c)\text{ for }c\ne 0$

- Hence, two points $\hat{\textbf{p}}, \hat{\textbf{q}}\in \mathbb{R}^3\ \{O\}$ describe the same point in 2D (and line in 3D) if $\hat{\textbf{p}}=\lambda\hat{\textbf{q}}$ for some $\lambda\ne 0$

---

**Translation in Homogeneous Coordinates**

> But… How does help translations?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117190914490.png" alt="image-20250117190914490" style="zoom:50%;" />

It looks like shear transformation! (Again, highly recommend to watch the original video)

But wait a minute—shear is a linear transformation!

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117191234583.png" alt="image-20250117191234583" style="zoom:50%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117191558353.png" alt="image-20250117191558353" style="zoom:50%;" />

> This is magic…
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117191726019.png" alt="image-20250117191726019" style="zoom:33%;" />

$$
\text{\color{blue}{Now easy to compose all these transformation!}}
$$

**3D transformations in Homogeneous Coordinates**

Just append one "homogeneous coordinate" to the first three

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117192144826.png" alt="image-20250117192144826" style="zoom:50%;" />

> [!WARNING]
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117192303435.png" alt="image-20250117192303435" style="zoom:50%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117192601550.png" alt="image-20250117192601550" style="zoom:50%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117193018916.png" alt="image-20250117193018916" style="zoom:70%;" />

---

**Perspective Projection in Homogeneous Coordinates**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117193347173.png" alt="image-20250117193347173" style="zoom:50%;" />

---

**Scene Graph**

For complex scenes (e.g., more than just a cube!), <u>scene graph</u> can help organize transformations

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117194441434.png" alt="image-20250117194441434" style="zoom:50%;" />

---

**Instances**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117195007090.png" alt="image-20250117195007090" style="zoom:50%;" />

---

**Order matters when composing transformations!**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117195151546.png" alt="image-20250117195151546" style="zoom:50%;" />

---

**Summary**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250117195614739.png" alt="image-20250117195614739" style="zoom:50%;" />

**3D Rotations—Degrees of Freedom**

We only need three degrees of freedom to define a 3D rotation transformation:

- Rotation axis: normalized vector, only $x$ and $y$

  > We don't need the $z$ because the constraint $\sqrt{x^2+y^2+z^2}=1$

- Rotation angle

---

**Representing Rotations in 3D—Euler Angles**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122155341231.png" alt="image-20250122155341231" style="zoom:50%;" />

**Gimbal Lock**

When using Euler angles $\theta_x, \theta_y, \theta_z$, may reach a configuration where there is no way to rotate around one of the three axes!

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122161128590.png" alt="image-20250122161128590" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122161351692.png" alt="image-20250122161351692" style="zoom:50%;" />

**Rotation from Axis/Angle**

> Due to Gimbal Lock, using Euler Angles to present the rotation is not great. What we gonna do?

We can perform rotation around a given axis $\mathbf{u}$ by a given angle $\theta$ and we will use the **complex number**.
$$
\text{\color{red}{DON'T: Think of these numbers as "complex".}}\\
\text{\color{red}{DO: Imagine we're simply defining additional operations (like dot and cross).}}
$$

---

**Imaginary Unit**

Imaginary Unit: $i\coloneqq\sqrt{-1}$

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122162817240.png" alt="image-20250122162817240" style="zoom:33%;" />

The **geometric description**: Imaginary unit is just a quarter-turn in the counter-clockwise direction.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122163124799.png" alt="image-20250122163124799" style="zoom:50%;" />

---

**Complex Numbers**

They are just 2-vectors

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122163307597.png" alt="image-20250122163307597" style="zoom:50%;" />

---

**Complex Arithmetic**

- Same operations as before, plus one more:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122163909516.png" alt="image-20250122163909516" style="zoom:50%;" />

- Complex multiplication:
  - angles add
  - magnitudes multiply

---

**Complex Product—Polar Form**

- Perhaps the most beautiful identity in math:
  $$
  e^{i\pi}+1=0
  $$

- Specialization of Euler's formula:

  > [!TIP]
  >
  > Imagine this as rotate around the circle $x^2 + y^2 =1$ with angle $\theta$

$$
e^{i\theta}=cos(\theta)+i\ sin(\theta)
$$

- Can use to "implement" complex product: (indicators refer to the last image)
  $$
  z_1=r_1e^{i\theta_1},\space z_2=r_2e^{i\theta_2}\\
  z_1z_2=r_1r_2e^{i(\theta_1+\theta_2)}\\
  \text{Attention: 1. angles add; 2. magnitudes multiply}
  $$

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122171234805.png" alt="image-20250122171234805" style="zoom:33%;" />

---

**Quaternions**

- TLDR: Kind of like complex numbers but for 3D rotations
- Weird situation: can't do 3D rotations w/ only 3 components!

**Quaternions in Coordinates**

In order to do 3D rotations in a way that mimics complex numbers for 2D, actually need **FOUR** coordinates.
$$
\mathbb{H}\coloneqq span(\{1,i,j,k\})\\
q=a+bi+cj+dk\in\mathbb{H}\\
i^2=j^2=k^2=ijk=-1
$$
<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122173052057.png" alt="image-20250122173052057" style="zoom:50%;" />

> [!warning]
>
> Product is no longer commutive!
> $$
> \text{For }q,p\in\mathbb{H},\space qp\ne pq
> $$

---

**Quaternion Product in Components**

Given two quaternions:
$$
q=a_1+b_1i+c_1j+d_1k\\
p=a_2+b_2i+c_2j+d_2k
$$
the product

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122173912014.png" alt="image-20250122173912014" style="zoom:50%;" />

---

**Quaternions—Scalar + Vector Form**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122174249018.png" alt="image-20250122174249018" style="zoom:50%;" />

---

**3D Transformations via Quaternions—Rotations (main use)**

Given axis $\mathbf{u}$, angle $\theta$, quaternion $\mathbf{q}$ representing rotation is:
$$
q=\cos(\theta/2)+\sin(\theta/2)\mathbf{u}\\
\vdot =\bar{q}xq\\
$$
the $\bar{q}$ is the conjugate complex number (共轭复数)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250210172818186.png" alt="image-20250210172818186" style="zoom:50%;" />

---

**Interpolating Rotations**—<u>*Slerp*</u>

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122190122156.png" alt="image-20250122190122156" style="zoom:50%;" />

---

**Summary**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250122192110794.png" alt="image-20250122192110794" style="zoom:50%;" />

> [!important]
> $$
> \text{\color{red}{Highly Recommended!}}
> $$
> [quaternion.pdf](https://krasjet.github.io/quaternion/quaternion.pdf)

# Lecture 07:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250224154718666.png" alt="image-20250224154718666" style="zoom:50%;" />

**View Frustum**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250224155604059.png" alt="image-20250224155604059" style="zoom:50%;" />

> Keep in mind that camera is always located in the original and look forward to the $-z$

---

**Clipping**

"Clipping" eliminated triangles not visible to the camera / in view frustum.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250224160048696.png" alt="image-20250224160048696" style="zoom:50%;" />

---

**Near/Far Clipping**

We clip out too far and too near part to avoid "**Z-fighting**".

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250224161200750.png" alt="image-20250224161200750" style="zoom:50%;" />

---

**Mapping frustum to unit cube**

> 注意看，这里图有一点问题，灰色部分其实还是一个长方体而非锥体

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250224162701364.png" alt="image-20250224162701364" style="zoom:70%;" />

> You can calculate this manually, it is kind of wonderful.

---

**Matrix for Perspective Transform**

> [透视投影变换详解-CSDN博客](https://blog.csdn.net/popy007/article/details/1797121)
>
> 投影在Near平面上，lbrt都是对于Near平面而言，流程：
>
> 1. 计算在Near平面上的投影坐标
> 2. 将Near平面transform到unit cube中，这一步里面对于z轴的处理是立体的，因为要保留z的信息，但是对于xy的处理就不涉及z了（但是最后矩阵乘完了之后要除以z做一个透视变换）

###### <img src="D:\Desktop\image-20250224164925882.png" alt="image-20250224164925882" style="zoom:50%;" /><img src="D:\WeChat\data\WeChat Files\wxid_5vi1eegin94x22\FileStorage\Temp\eb0e876a2bd1282defc85fea5b882db.png" alt="eb0e876a2bd1282defc85fea5b882db" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250303215636995.png" alt="image-20250303215636995" style="zoom:50%;" />

---

> Review: screen transformation
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250224165616348.png" alt="image-20250224165616348" style="zoom:50%;" />

**Transformations: From Objects to the Screen**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250224165941944.png" alt="image-20250224165941944" style="zoom:33%;" />

---

$$
\text{\color{blue}{So, how do we draw nice primitives?}}
$$

Consider sampling color(x, y)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250224171943262.png" alt="image-20250224171943262" style="zoom:50%;" />

---

**Linear interpolation in 2D**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226171032964.png" alt="image-20250226171032964" style="zoom:50%;" />

> Review: 1D linear interpolation
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226171138182.png" alt="image-20250226171138182" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226171312303.png" alt="image-20250226171312303" style="zoom:50%;" />

Of course there are many ways to do 2D interpolation for the triangle.

---

**Barycentric Coordinates**

- No matter how you computer them, the values of the three functions $\phi_i(x),\phi_j(x),\phi_k(x)$ for a given point are called <u>barycentric coordinates</u>.
- Can be used to interpolate any attribute associated with vertices (color, texture coordinates, etc.)
- **Importantly, these same three values fall out of the half-plane tests used for triangle rasterization!**
  - Because we use the cross product to test whether the point is inside a triangle. And the **cross product is actually the area**!
- Hence, get them for "free" during rasterization

---

**Perspective-incorrect interpolation**

Due to perspective projection (homogeneous divide), barycentric interpolation of values on a triangle with different depths <u>is not</u> an affine function of screen XY coordinates.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226173000824.png" alt="image-20250226173000824" style="zoom:50%;" />
$$
\text{\textcolor{red}{Want to interpolate attribute values linearly in 3D object space, not image space.}}\\
\text{\textcolor{red}{If we compute barycentric coordinates using 2D (projected) coordinates, leads to (derivative) discontinuity in interpolation where quad was split.}}
$$
<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226174117105.png" alt="image-20250226174117105" style="zoom:50%;" />

---

**Perspective Correct Interpolation**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226191226227.png" alt="image-20250226191226227" style="zoom:50%;" />

> For derivation: [Microsoft Word - lowk_persp_interp_06.doc](https://www.comp.nus.edu.sg/~lowkl/publications/lowk_persp_interp_techrep.pdf)

---

**Texture Mapping**

Map a picture to the object's surface instead of simply using color.

---

**Normal & Displacement Mapping**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226192734841.png" alt="image-20250226192734841" style="zoom:50%;" />

> Explanation by DeepSeek: https://yuanbao.tencent.com/bot/app/share/chat/a264d657147adddf88d4c7c6164e419d

---

**Texture Coordinates**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226193905076.png" alt="image-20250226193905076" style="zoom:50%;" />

When modeling, each vertex will contain a texture coordinates refer to the texture map ($u$ and $v$).

E.g.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226194320766.png" alt="image-20250226194320766" style="zoom:50%;" />

> Texture Sponza
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226195919526.png" alt="image-20250226195919526" style="zoom:40%;" />
>
> Periodic Texture
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226195940806.png" alt="image-20250226195940806" style="zoom:40%;" />

---

**Texture Sampling 101**

> 在确定了哪些像素点在三角形内部之后，对三角形的三个顶点的uv坐标（这个是顶点建模的时候储存的）进行插值获得这个像素点的uv坐标

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226200205105.png" alt="image-20250226200205105" style="zoom:50%;" />

---

**Aliasing**

> Recall aliasing: undersampling a high-frequency signal can result in aliasing
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226200700874.png" alt="image-20250226200700874" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226200916642.png" alt="image-20250226200916642" style="zoom:50%;" />

Explanation about why aliasing:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226200947718.png" alt="image-20250226200947718" style="zoom:50%;" />

---

**Magnification (which is easier to generate aliasing) vs. Minification**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226201047285.png" alt="image-20250226201047285" style="zoom:50%;" />

> reciprocal relationships are sometimes a bit counter-intuitive at first: when you get *close* to an object (magnification), one screen pixel becomes *small* relative to the texture. And vice versa.

$$
\text{\textcolor{blue}{How to mitigate them?}}
$$

---

> $(u,v)$ is not always an integer, since we calculated it via interpolation

**Bilinear Interpolation** (Magnification)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226202037630.png" alt="image-20250226202037630" style="zoom:50%;" />

---

**Texture Prefiltering** (Minification)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226202324232.png" alt="image-20250226202324232" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226202513196.png" alt="image-20250226202513196" style="zoom:50%;" />

---

$$
\text{\textcolor{blue}{But how many resolutions we need?}}
$$

**MIP Map**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226203526483.png" alt="image-20250226203526483" style="zoom:50%;" />

To store the mipmap, the memory overhead is only one third of original image (the left up part):

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226203610480.png" alt="image-20250226203610480" style="zoom:50%;" />

---

**Computing MIP Map Level**

> 当距离过远的时候，一个像素可能在texture map中占据了很大一部分。这个时候，越远所使用的texture map就越模糊，这样同样的像素点占据的texture map中的像素就越少，aliasing现象就会减弱

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226203840235.png" alt="image-20250226203840235" style="zoom:50%;" />

> u and v are both texture map coordinates, don't be misunderstood by the figure since it annotated the u and v in screen coordinate

Problem about MIP map:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226204658637.png" alt="image-20250226204658637" style="zoom:50%;" />

> 人话：mipmap的level d是一个离散值（其实是取整之后为离散值），这样就会出现在d切换的交界处十分生硬，所以想要使用连续值而非离散值来表示d，但是这又不太可能。折衷的方法就是在d靠近的两个level之间做插值。之前是双线性插值，现在是三线性插值。

> **Trilinear Filtering**
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226205021513.png" alt="image-20250226205021513" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226205128709.png" alt="image-20250226205128709" style="zoom:67%;" />

---

**Anisotropic Filtering**

> Refer to page 19 of these slides: [Microsoft PowerPoint - Slides (2).ppt](https://cgl.ethz.ch/teaching/former/vc_master_06/Downloads/Mipmaps_1.pdf)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226205503910.png" alt="image-20250226205503910" style="zoom:50%;" />

---

**Summary**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226205700912.png" alt="image-20250226205700912" style="zoom:67%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250226205749196.png" alt="image-20250226205749196" style="zoom:67%;" />

# Lecture 08

**Depth Buffer**

Only update the color buffer and depth buffer when the depth $z$ is smaller than the one in the depth buffer.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227170743247.png" alt="image-20250227170743247" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227170757960.png" alt="image-20250227170757960" style="zoom:50%;" />

> When super-sampling, store one depth value per (super) sample — not one per pixel!

---

**Compositing**

Use alpha to represent the transparency / opacity:
<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227171228422.png" alt="image-20250227171228422" style="zoom:50%;" />

---

**Fringing**

Poor treatment of color/alpha can yield dark "fringing".

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227171900601.png" alt="image-20250227171900601" style="zoom:40%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227171916346.png" alt="image-20250227171916346" style="zoom:40%;" />

---

**Over operator**

which is <u>not commutative</u>.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227172028646.png" alt="image-20250227172028646" style="zoom:50%;" />

This is a non-pre-multiplied alpha, which always leads to fringing

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227172107494.png" alt="image-20250227172107494" style="zoom:50%;" />

---

**Pre-multiplied alpha**

Major difference:

1. multiple the alpha into color at the very first
2. divide the new alpha after the "over" operation

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227172356034.png" alt="image-20250227172356034" style="zoom:50%;" />

Why do we need it? Examples:
Super-sampling:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227172532981.png" alt="image-20250227172532981" style="zoom:50%;" />

Pre-filtering

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227173901623.png" alt="image-20250227173901623" style="zoom:50%;" />

"Over" repeatedly

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227174138238.png" alt="image-20250227174138238" style="zoom:50%;" />

Advantages of pre-multiplied alpha

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227174209123.png" alt="image-20250227174209123" style="zoom:50%;" />

---

**Drawing semi-transparent primitives** can always be painful, since the "over" operation is not commutative.

> Depth peeling (order independent transparency): [OrderIndependentTransparency.pdf](https://developer.download.nvidia.com/assets/gamedev/docs/OrderIndependentTransparency.pdf)

Normal solution:

1. render opaque primitives first (in any order)
2. render semi-transparent surfaces <u>in back-to-front order</u>

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227174757878.png" alt="image-20250227174757878" style="zoom:50%;" />

---

# **Summary: Rasterization**

Goal: turn inputs into an image!

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175000878.png" alt="image-20250227175000878" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175014768.png" alt="image-20250227175014768" style="zoom:33%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175029903.png" alt="image-20250227175029903" style="zoom:33%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175054184.png" alt="image-20250227175054184" style="zoom:33%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175129779.png" alt="image-20250227175129779" style="zoom:33%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175221759.png" alt="image-20250227175221759" style="zoom:33%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175234384.png" alt="image-20250227175234384" style="zoom:33%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175303546.png" alt="image-20250227175303546" style="zoom:33%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175315010.png" alt="image-20250227175315010" style="zoom:33%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250227175324959.png" alt="image-20250227175324959" style="zoom:33%;" />

















