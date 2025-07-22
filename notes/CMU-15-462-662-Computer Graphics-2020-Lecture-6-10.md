

[TOC]

# Lecture 06:

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



<img src="D:\Desktop\image-20250224164925882.png" alt="image-20250224164925882" style="zoom:50%;" />



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

# Lecture09: Geometry

**What is geometry?**

1. The study of shapes, sizes, patterns, and positions.
2. The study of spaces where some quantity (lengths, angles, etc.) can be *measured*.

---

**How can we describe geometry?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310135526796.png" alt="image-20250310135526796" style="zoom:50%;" />

Remember, there is not the "best" option!

Basically, there are two ways:

- Explicit
- Implicit

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310135744045.png" alt="image-20250310135744045" style="zoom:50%;" />

---

**"Implicit" representations of geometry**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310135826732.png" alt="image-20250310135826732" style="zoom:50%;" />

Examples:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310140035434.png" alt="image-20250310140035434" style="zoom:50%;" />

> - Implicit surfaces make some tasks hard (like sampling: to find the points located on the surface)
> - Implicit surfaces make other tasks easy (like inside/outside tests)

---

**"Explicit" representation of geometry**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310140519448.png" alt="image-20250310140519448" style="zoom:50%;" />

Examples:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310140554785.png" alt="image-20250310140554785" style="zoom:50%;" />

> - Explicit surfaces make some tasks easy (like sampling)
> - Explicit surfaces make other tasks hard (like inside/outside tests) 

$$
\text{\textcolor{red}{Conclusion: some representations work better that others---depends on the tasks!}}
$$

---

**Constructive Solid Geometry (Implicit)**

Boolean operations:

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310141407554.png" alt="image-20250310141407554" style="zoom:50%;" />

---

**Blobby Surfaces (Implicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310141751028.png" alt="image-20250310141751028" style="zoom:50%;" />

> There is a **bug** in this slide, actually what it did is make the distance between $p$ and $q$ smaller.

---

**Blending Distance Functions (Implicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310142236438.png" alt="image-20250310142236438" style="zoom:50%;" />

> 这里的$f(x)$为0的时候就是取值的时候

---

**Level Set Methods (Implicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310155511506.png" alt="image-20250310155511506" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310155953385.png" alt="image-20250310155953385" style="zoom:50%;" />

---

**Fractals (Implicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310160025154.png" alt="image-20250310160025154" style="zoom:50%;" />

Often use **Mandelbrot Set** to define the fractals

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310160117023.png" alt="image-20250310160117023" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310160127262.png" alt="image-20250310160127262" style="zoom:50%;" />

This is a loop, as long as the magnitude remains bounded (never goes to $∞$)

---

**Pros & Cons: Implicit Representations**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310160332886.png" alt="image-20250310160332886" style="zoom:50%;" />
$$
\text{Now, Let's talk about the explicit representations!}
$$

---

**Point Cloud (Explicit)** 

Which is dead easy.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310160520706.png" alt="image-20250310160520706" style="zoom:50%;" />

---

**Polygon Mesh (Explicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310161207095.png" alt="image-20250310161207095" style="zoom:50%;" />

---

**Triangle Mesh (Explicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310161458317.png" alt="image-20250310161458317" style="zoom:50%;" />

---

**Bernstein Basis**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310162117321.png" alt="image-20250310162117321" style="zoom:50%;" />

> 就是一种新的插值的系数，以往的插值系数都是线性的（$t$ 和 $1-t$这种），现在的插值系数不是线性的了

---

**Bezier Curves (Explicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310162415724.png" alt="image-20250310162415724" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310162921411.png" alt="image-20250310162921411" style="zoom:50%;" />

> **Piecewise Bezier Curves (Explicit)**
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310163105627.png" alt="image-20250310163105627" style="zoom:50%;" />

---

**Bezier Curves — tangent continuity**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310163309610.png" alt="image-20250310163309610" style="zoom:50%;" />

>  @sponge For each segment of a cubic Bezier curve, you have 4 control points. So, 4 "vector degrees of freedom." Each control point has a number of scalar degrees of freedom equal to the dimension (one for each coordinate), so 2 for points in the plane. So, 8 scalar degrees of freedom for each planar segment.
>
> I believe in the video I was talking about a loop made of two cubic Bezier segments, each of which has 4 vector degrees of freedom (for 8 total).

---

**Tensor Product**

> 为了后面的曲面，这里需要两个控制插值的参数（也就是$u$和$v$，在前面是$s$，线性插值中是$t$），注意这里的$u$和$v$不是texture map里面的那个
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310170449560.png" alt="image-20250310170449560" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310163705470.png" alt="image-20250310163705470" style="zoom:50%;" />

---

**Bezier Patches**

> 把面划分为网格，比如$3\times3$的网格，那么就会有九个影响点，也就是九个p，然后$u$和$v$分别控制横向和纵向，取值为0-1，其实这个本质上也就是在插值罢了
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310170602980.png" alt="image-20250310170602980" style="zoom:50%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310202552638.png" alt="image-20250310202552638" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310165143304.png" alt="image-20250310165143304" style="zoom:50%;" />

---

**Bezier Surface**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310165355451.png" alt="image-20250310165355451" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310202732923.png" alt="image-20250310202732923" style="zoom:50%;" />
>
> Instead of the Bezier Patch, there are lots of alternatives!
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310202851298.png" alt="image-20250310202851298" style="zoom:50%;" />

---

**Rational B-Splines (Explicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310203152464.png" alt="image-20250310203152464" style="zoom:50%;" />

> **多项式性质**：传统的Bézier曲线和B样条都是多项式曲线，其表达式是控制点与基函数（如Bernstein基函数或B样条基函数）的加权和。这类曲线在几何上有很多优点（如凸包性质和局部性），但它们都是纯多项式的，而圆和其他圆锥曲线不是多项式。
>
> **近似问题**：虽然可以用多段Bézier曲线来近似一个圆，但不能用一个单一的多项式精确表示圆或椭圆。 
>
> **Rational B-Spline** 是在普通B样条的基础上引入控制点权重和齐次坐标后得到的曲线或曲面表达方式。
>
> **为什么贝塞尔曲线不能画圆**：由于贝塞尔曲线（和普通B样条）是纯多项式的，它们无法精确表示圆或其他圆锥曲线；而通过引入权重，Rational B-Spline能以有理函数的形式精确描述这些几何形状。
>
> **齐次坐标的必要性**：使用齐次坐标可以方便地把权重整合进曲线表达中，并在最终计算时通过除权重得到准确的笛卡尔坐标，从而实现对复杂曲线的精确控制。

---

**NURBS (Explicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310203510031.png" alt="image-20250310203510031" style="zoom:50%;" />

<u>Homogeneous coordinates $w$ controls "strength" of  a vertex</u>

---

**NURBS Surface (Explicit)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310203624383.png" alt="image-20250310203624383" style="zoom:50%;" />

---

**Subdivision**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310203800431.png" alt="image-20250310203800431" style="zoom:50%;" />

How to choose weight for each split?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310203839251.png" alt="image-20250310203839251" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250310203916089.png" alt="image-20250310203916089" style="zoom:50%;" />

# Lecture 10: Meshes and Manifolds

**Why do we use the square grids?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312125412211.png" alt="image-20250312125412211" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312125428863.png" alt="image-20250312125428863" style="zoom:50%;" />

---

**Smooth Surfaces**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312125531429.png" alt="image-20250312125531429" style="zoom:50%;" />

*Surfaces are manifold.*

---

**Manifold**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312125010485.png" alt="image-20250312125010485" style="zoom:50%;" />

*Isn't every shape manifold?*

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312125647691.png" alt="image-20250312125647691" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312125706047.png" alt="image-20250312125706047" style="zoom:50%;" />

**A manifold polygon mesh has fans, not fins**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312125902640.png" alt="image-20250312125902640" style="zoom:50%;" />

> - Here "fans" means the faces around the vertex looks like a fan
> - Here "fins" means the two "fin" beside the vertex

---

**Boundary**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312130106353.png" alt="image-20250312130106353" style="zoom:50%;" />

---

**Polygon Soup**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312130258786.png" alt="image-20250312130258786" style="zoom:50%;" />

---

**Adjacency List (Array-like)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312130344352.png" alt="image-20250312130344352" style="zoom:50%;" />

---

**Incidence Matrices**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312130618677.png" alt="image-20250312130618677" style="zoom:50%;" />

> This is easy to find the neighbor. But it memory overhead is $O(n^2)$. There are too much $0$.

><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312130741298.png" alt="image-20250312130741298" style="zoom:50%;" />

---

**Halfedge Data Structure** (very useful)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312130913051.png" alt="image-20250312130913051" style="zoom:50%;" />

It can make mesh traversal easy

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312131032183.png" alt="image-20250312131032183" style="zoom:50%;" />

It can guarantee the manifold

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312131205346.png" alt="image-20250312131205346" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312131556332.png" alt="image-20250312131556332" style="zoom:50%;" />

---

**Halfedge meshes are easy to edit**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312131937034.png" alt="image-20250312131937034" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312132439744.png" alt="image-20250312132439744" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312132542324.png" alt="image-20250312132542324" style="zoom:50%;" />

> here is a typo. (b, c) -> (c, d)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250312132757690.png" alt="image-20250312132757690" style="zoom:50%;" />

























