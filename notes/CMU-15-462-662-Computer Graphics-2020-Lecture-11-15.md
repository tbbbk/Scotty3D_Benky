[TOC]

# Lecture 11: Digital Geometry Processing

**What is geometry processing?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313144533594.png" alt="image-20250313144533594" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313144826613.png" alt="image-20250313144826613" style="zoom:50%;" />

---

**Geometry Processing: Reconstruction**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145018208.png" alt="image-20250313145018208" style="zoom:50%;" />

---

**Geometry Processing: Upsampling**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145113836.png" alt="image-20250313145113836" style="zoom:50%;" />

---

**Geometry Processing: Downsampling**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145146320.png" alt="image-20250313145146320" style="zoom:50%;" />

---

**Geometry Processing: Resampling**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145221656.png" alt="image-20250313145221656" style="zoom:50%;" />

---

**Geometry Processing: Filtering**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145259229.png" alt="image-20250313145259229" style="zoom:50%;" />

---

**Geometry Processing: Compression**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145327509.png" alt="image-20250313145327509" style="zoom:50%;" />

---

**Geometry Processing: Shape Analysis**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145406138.png" alt="image-20250313145406138" style="zoom:50%;" />
$$
\text{\textcolor{blue}{Enough overview, let's process some geometry!}}
$$

---

**Remeshing as resampling**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145705914.png" alt="image-20250313145705914" style="zoom:50%;" />

> But what makes a "good" mesh?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145732806.png" alt="image-20250313145732806" style="zoom:50%;" />

Main idea: *good approximation of original shape!*

However…

---

**Approximation of position is not enough!**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313145813949.png" alt="image-20250313145813949" style="zoom:50%;" />

**What else makes a "good" triangle mesh?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313150106681.png" alt="image-20250313150106681" style="zoom:50%;" />

Main idea: *Delaunay*

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313150428080.png" alt="image-20250313150428080" style="zoom:100%;" />

> 这里的degree：一个点围绕有几个三角形

$$
\text{\textcolor{red}{End of resampling. How can we upsampling?}}
$$

---

**Upsampling via Subdivision**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313150722416.png" alt="image-20250313150722416" style="zoom:50%;" />

---

**Catmull-Clark Subdivision**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313151115865.png" alt="image-20250313151115865" style="zoom:50%;" />

> If still not understand, read this: [Catmull-Clark Subdivision: The Basics – CodeItNow](https://www.rorydriscoll.com/2008/08/01/catmull-clark-subdivision-the-basics/)

Catmull-Clark subdivision works on every shape of mesh, but best for quad mesh. And usually it is not suitable for triangle mesh

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313152915178.png" alt="image-20250313152915178" style="zoom:33%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313152903055.png" alt="image-20250313152903055" style="zoom:33%;" />

---

**Loop Subdivision**

For triangle mesh subdivision, we have loop subdivision

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313153028742.png" alt="image-20250313153028742" style="zoom:50%;" />

>  So we use the left figure's weight to decide the position of the vertex that we newly inserted in, and use the right figure's weight to decide the position of the vertex that originally existed in the polygon.

> We can also use the edge operation to implement the loop subdivision
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313153440376.png" alt="image-20250313153440376" style="zoom:50%;" />
>
> MENTION: this only updated the connectivity, so remember to update the vertex positions!

$$
\text{\textcolor{red}{Among, we make more tirangles, but what if we wanna fewer triangles?}}
$$

---

**Simplification via Edge Collapse**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313154035634.png" alt="image-20250313154035634" style="zoom:50%;" />

Basically, we assign every edge a value, and iteratively delete the least valued edge.

For this operation, <u>quadric error metric</u> is a effective cost (value) function

---

**Quadric Error Metric**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313154654057.png" alt="image-20250313154654057" style="zoom:50%;" />

$<n, x>$是$x$沿着法线到原点的距离，$<n, p>$是$p$沿着法线到原点的距离，但是由于$p$在平面上，所以$p$就是平面到原点的距离，所以$<n, x> - <n, p>$就是$x$到平面的距离

---

**Quadric Error with Homogeneous Coordinates**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313160140853.png" alt="image-20250313160140853" style="zoom:50%;" />

> Here is a typo: $<u,v>$ should be $ax+by+cz-d$ 

---

**Quadric Error of Edge Collapse**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313160638329.png" alt="image-20250313160638329" style="zoom:50%;" />

$K_i$ and $K_j$ are the quadrics stored at vertices $i$ and $j$ (resp.) before the collapse. These vertex quadrics could be the sum of quadrics from all triangles around those vertices, or they could result from earlier edge collapses.

---

**Minimize Quadratic Polynomial**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313160811271.png" alt="image-20250313160811271" style="zoom:50%;" />

When the $A$ is positive definite, the point with zero derivative is the minimal

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313161155869.png" alt="image-20250313161155869" style="zoom:50%;" />

**Minimize Quadratic Error**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313161217766.png" alt="image-20250313161217766" style="zoom:50%;" />

The final algorithm!

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313161608127.png" alt="image-20250313161608127" style="zoom:67%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313161755371.png" alt="image-20250313161755371" style="zoom:50%;" />
>
> > 这里的 **normal** 指的是**三角形的法向量（surface normal）**，即垂直于三角形平面的单位向量。
> >
> > **法向量在 Quadric Simplification 里的作用**
> >
> > 在 **Quadric Error Metric (QEM) 简化网格** 时，我们通常**合并**两个顶点，这可能会导致某些三角形的法向量“翻转”（flipping），即它的方向发生了变化，导致网格表面错误。
> >
> > 如 PPT 所示：
> >
> > 1. **左侧网格（原始）**：黑色边代表要被合并的边。
> > 2. **中间网格（正确合并）**：新顶点的选择没有造成法向量翻转。
> > 3. **右侧网格（错误合并）**：某些三角形（红色）翻转，导致法向量方向错误。
> >
> > **如何检测法向量翻转？**
> >
> > 关键观察是：
> >
> > - 在合并某条边时，我们需要检查**相邻两个三角形的法向量**（比如 NijkN_{ijk} 和 NkjlN_{kjl}）。
> > - 如果它们的**点积** ⟨Nijk,Nkjl⟩\langle N_{ijk}, N_{kjl} \rangle 为**负数**，说明它们的夹角大于 90°，即**其中一个三角形的法向量发生了翻转**，导致错误。
> >
> > **解决方案**
> >
> > - **若 ⟨Nijk,Nkjl⟩<0\langle N_{ijk}, N_{kjl} \rangle < 0**（法向量翻转），则**不合并这条边**。
> > - 这样可以防止网格拓扑错误，保证网格表面光滑且法向量方向一致。

---

**More Delaunay**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313162421136.png" alt="image-20250313162421136" style="zoom:50%;" />

**Improve Degree**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313162634076.png" alt="image-20250313162634076" style="zoom:50%;" />

**More Round**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313162719385.png" alt="image-20250313162719385" style="zoom:50%;" />

---

**Isotropic Remeshing Algorithm**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250313162812054.png" alt="image-20250313162812054" style="zoom:50%;" />

# Lecture 12: Geometric Queries

**What is geometric queries?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401165528819.png" alt="image-20250401165528819" style="zoom:50%;" />

**Example: Find the closest point**

- Point to point

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401165810318.png" alt="image-20250401165810318" style="zoom:50%;" />

- Closest point on line

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401165842267.png" alt="image-20250401165842267" style="zoom:50%;" />

- Closest point on line segment

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401165908219.png" alt="image-20250401165908219" style="zoom:50%;" />

- Closest point on triangle

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401170018156.png" alt="image-20250401170018156" style="zoom:50%;" />

- Closest point on triangle in 3D

  <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401170058789.png" alt="image-20250401170058789" style="zoom:50%;" />

---

**Closest point on triangle mesh in 3D**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401170306194.png" alt="image-20250401170306194" style="zoom:50%;" />

*Looping over all faces is too naive and time-costing.*

---

**Closest point to implicit surface**

Use the gradient of distance

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401170803091.png" alt="image-20250401170803091" style="zoom:50%;" />

---

**Ray-mesh Intersection**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401170837241.png" alt="image-20250401170837241" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401172029854.png" alt="image-20250401172029854" style="zoom:50%;" />

**Ray-implicit surface intersection**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401172202227.png" alt="image-20250401172202227" style="zoom:50%;" />

**Ray-plane Intersection**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401172441661.png" alt="image-20250401172441661" style="zoom:50%;" />

**Ray-triangle Intersection**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401172609563.png" alt="image-20250401172609563" style="zoom:50%;" />

**Mesh-mesh Intersection**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401172837125.png" alt="image-20250401172837125" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401172914491.png" alt="image-20250401172914491" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401172933530.png" alt="image-20250401172933530" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250401173146669.png" alt="image-20250401173146669" style="zoom:50%;" />

# Lecture 13: Spatial Data Structures

We wanna perform the geometric query on a scene <u>effeciently</u>!

> Review: ray-triangle intersection
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402165001095.png" alt="image-20250402165001095" style="zoom:50%;" />

**First Hit Problem**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402165454719.png" alt="image-20250402165454719" style="zoom:50%;" />

---

**Naive algorithm**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402165509800.png" alt="image-20250402165509800" style="zoom:50%;" />

---

**Bounding Box**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402165546522.png" alt="image-20250402165546522" style="zoom:50%;" />

How do we know the ray intersects with the plane?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402165831408.png" alt="image-20250402165831408" style="zoom:50%;" />

> We can learn from the binary search!
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402165930540.png" alt="image-20250402165930540" style="zoom:50%;" />

With better data structure, we can process geometric queries faster!

---

**Bounding volume hierarchy (BVH)**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402170405367.png" alt="image-20250402170405367" style="zoom:50%;" />

The child boxes may overlap in space. Intuitively, the more they overlap, the worse the BVH is.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402170505671.png" alt="image-20250402170505671" style="zoom:50%;" />

> Given a set of primitives, there are many possible BVHs…
>
> How do we <u>quickly</u> build a high-quality BVH? Below which is the better partition?
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402170812374.png" alt="image-20250402170812374" style="zoom:50%;" />
>
> We want small bounding boxes (minimize overlap between children, avoid empty sapce.)

**A good partitioning minimizes the <u>cost</u> of finding the closest intersection of a tray with primitives in the node.**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402171016054.png" alt="image-20250402171016054" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402171033133.png" alt="image-20250402171033133" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402171425470.png" alt="image-20250402171425470" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402171419320.png" alt="image-20250402171419320" style="zoom:50%;" />

**Implementing Partitions**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402171542399.png" alt="image-20250402171542399" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402171704732.png" alt="image-20250402171704732" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402171856539.png" alt="image-20250402171856539" style="zoom:67%;" />

However… There are some troublesome cases.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402172015339.png" alt="image-20250402172015339" style="zoom:50%;" />

---

Till now, we are partitioning primitives, how about partitioning the space?

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402172131488.png" alt="image-20250402172131488" style="zoom:50%;" />

---

K-D tree

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402172236553.png" alt="image-20250402172236553" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402172509832.png" alt="image-20250402172509832" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402172556515.png" alt="image-20250402172556515" style="zoom:50%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402172719273.png" alt="image-20250402172719273" style="zoom:50%;" />
>
> For this situation, the first hit is not in the pink node, which means the pink node is not the "terminate" node. It still need to forward.

---

**Uniform Grid**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402172743382.png" alt="image-20250402172743382" style="zoom:50%;" />

The grid resolution is a important argument

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402172849372.png" alt="image-20250402172849372" style="zoom:50%;" />

> ![image-20250402173313794](C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402173313794.png)<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402173324017.png" alt="image-20250402173324017" style="zoom:50%;" />

Uniform grid cannot adapt to non-uniform distribution of geometry in scene

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402173209556.png" alt="image-20250402173209556" style="zoom:50%;" />

---

**Quad tree/octree**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402173529555.png" alt="image-20250402173529555" style="zoom:50%;" />

**Summary of spatial acceleration structures**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402173650309.png" alt="image-20250402173650309" style="zoom:50%;" />

---

***How to use ray-intersection to rasterize an image?***

> Recall:
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402173855167.png" alt="image-20250402173855167" style="zoom:50%;" />

In the past, we solved this problem using rasterization plus depth buffering.

But we can also do it via ray queries!

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402174019550.png" alt="image-20250402174019550" style="zoom:50%;" />

**The comparasion**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402174337960.png" alt="image-20250402174337960" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402174734985.png" alt="image-20250402174734985" style="zoom:75%;" />

**Ray casting can be used for more tasks!**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250402174502971.png" alt="image-20250402174502971" style="zoom:50%;" />

# Lecture 14: Color

**What is color**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423150117103.png" alt="image-20250423150117103" style="zoom:50%;" />

**Heat generates color**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423150258889.png" alt="image-20250423150258889" style="zoom:50%;" />

**Spectrum**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423150338891.png" alt="image-20250423150338891" style="zoom:50%;" />

White light is really a mixture of all (visible) frequencies

**Additive vs. Subtractive Models of Light**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423150513452.png" alt="image-20250423150513452" style="zoom:50%;" />

**Emission Spectrum**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423150609986.png" alt="image-20250423150609986" style="zoom:50%;" />

**Absorption Spectrum**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423150718761.png" alt="image-20250423150718761" style="zoom:50%;" />

> Fundamental description of color: intensity or absorption as a function of frequency
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423150753288.png" alt="image-20250423150753288" style="zoom:50%;" />

**Interaction of emission and reflection**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423150939832.png" alt="image-20250423150939832" style="zoom:50%;" />

> Color reproduction is hard!
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423151008121.png" alt="image-20250423151008121" style="zoom:50%;" />

$$
\text{How does electromagnetic radiation (with a given power distribution)}\\\text{end up being \textcolor{red}{perceived by} a human as a certain color?}
$$

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423151356562.png" alt="image-20250423151356562" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423151408313.png" alt="image-20250423151408313" style="zoom:50%;" />

**Rods/Cone cell**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423151724617.png" alt="image-20250423151724617" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423151921536.png" alt="image-20250423151921536" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423152224731.png" alt="image-20250423152224731" style="zoom:50%;" />

**The human visual system**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423152415628.png" alt="image-20250423152415628" style="zoom:50%;" />

**Metamers**

Convenient for reproduce  a special color

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423152601243.png" alt="image-20250423152601243" style="zoom:50%;" />

**Color Spaces and Color Models**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423152758877.png" alt="image-20250423152758877" style="zoom:50%;" />

**Additive vs. Subtractive Color Models**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423152845741.png" alt="image-20250423152845741" style="zoom:50%;" />

> CMYK: the K stands for black. Generate pure black is really complicated. For convenience, why not just add the black into it.

> Other Common Color Models:
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423153044245.png" alt="image-20250423153044245" style="zoom:50%;" />
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423153521817.png" alt="image-20250423153521817" style="zoom:50%;" />

**How to encode RGB colors with hexadecimal values**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423153422744.png" alt="image-20250423153422744" style="zoom:50%;" />

**Why do we need different color models?**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423153547528.png" alt="image-20250423153547528" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423154223417.png" alt="image-20250423154223417" style="zoom:50%;" />

**Y'CbCr color model for compression**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423153619366.png" alt="image-20250423153619366" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423153939849.png" alt="image-20250423153939849" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423153944960.png" alt="image-20250423153944960" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423153953656.png" alt="image-20250423153953656" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423154011115.png" alt="image-20250423154011115" style="zoom:25%;" />

**CIE 1931* Color Space**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423154307961.png" alt="image-20250423154307961" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423154358346.png" alt="image-20250423154358346" style="zoom:50%;" />

**sRGB Color Space**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423154437002.png" alt="image-20250423154437002" style="zoom:50%;" />

**Color Acuity**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423154517112.png" alt="image-20250423154517112" style="zoom:50%;" />

**Color Conversion**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423154824030.png" alt="image-20250423154824030" style="zoom:50%;" />

**Gamma Correction**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250423154939124.png" alt="image-20250423154939124" style="zoom:50%;" />

# Lecture 15: Radiometry

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424140638779.png" alt="image-20250424140638779" style="zoom:50%;" />

A ray of light is a bunch of photons.

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424140736996.png" alt="image-20250424140736996" style="zoom:50%;" />

---

**Radiant energy** is total # of photons hits

**Radiant flux** is "hits per second"

**Irradiance** is total "hits per second per unit area"

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424141130282.png" alt="image-20250424141130282" style="zoom:50%;" />

---

**Unit**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424142454653.png" alt="image-20250424142454653" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424142510589.png" alt="image-20250424142510589" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424142521168.png" alt="image-20250424142521168" style="zoom:25%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424142529001.png" alt="image-20250424142529001" style="zoom:50%;" />

---

**Image generation as irradiance estimation**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424141120862.png" alt="image-20250424142529001" style="zoom:50%;" />

---

**Spectral power distribution**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150006303.png" alt="image-20250424150006303" style="zoom:50%;" />

---

> Why do some parts of this image look lighter or darker
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150039485.png" alt="image-20250424150039485" style="zoom:50%;" />

Because the angle!

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150127357.png" alt="image-20250424150127357" style="zoom:50%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150135434.png" alt="image-20250424150135434" style="zoom:50%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150151657.png" alt="image-20250424150151657" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250428145558942.png" alt="image-20250428145558942" style="zoom:70%;" />

---

**"N-dot-L" lighting**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150249998.png" alt="image-20250424150249998" style="zoom:50%;" />

---

**Irradiance falloff with distance**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150418691.png" alt="image-20250424150418691" style="zoom:50%;" />

---

**Angle and solid angles**: radians and steradians

the unit for solid angle is sr (steradians)

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150515740.png" alt="image-20250424150515740" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150558926.png" alt="image-20250424150558926" style="zoom:50%;" />

---

**Differential solid angle**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424150806799.png" alt="image-20250424150806799" style="zoom:50%;" />

> 这个小区域的边长就是角度乘半径，$\theta=\frac{l}{r}$

---

**Radiance**

It is the solid angle density of irradiance

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424151930857.png" alt="image-20250424151930857" style="zoom:50%;" />

**Surface Radiance**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424151953574.png" alt="image-20250424151953574" style="zoom:50%;" />

---

**Spectral Radiance**

radiant energy per unit time per unit area per solid angle 

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424152025609.png" alt="image-20250424152025609" style="zoom:50%;" />

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424154018705.png" alt="image-20250424154018705" style="zoom:67%;" />

---

**Light Field**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424160021994.png" alt="image-20250424160021994" style="zoom:50%;" />

---

Incident vs. Exitant Radiance

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424160050007.png" alt="image-20250424160050007" style="zoom:50%;" />

> Why we need radiance?
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424160126860.png" alt="image-20250424160126860" style="zoom:50%;" />

---

**Irradiance from the environment**

所有光源在这个小平面dA上形成的**irradiance**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424160154519.png" alt="image-20250424160154519" style="zoom:50%;" />

---

**Ambient occlusion**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424160816538.png" alt="image-20250424160816538" style="zoom:50%;" />

> Example:
>
> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424160858876.png" alt="image-20250424160858876" style="zoom:25%;" /><img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424160913969.png" alt="image-20250424160913969" style="zoom:25%;" />

---

**Irradiance from a uniform area source**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424161049436.png" alt="image-20250424161049436" style="zoom:50%;" />

> https://chatgpt.com/share/6809f2e9-60c8-8007-aaa5-b179590705d2

---

**Illumination**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424161649533.png" alt="image-20250424161649533" style="zoom:50%;" />

---

**Goniometry**

<img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424161854495.png" alt="image-20250424161854495" style="zoom:50%;" />

> <img src="C:\Users\tbk\AppData\Roaming\Typora\typora-user-images\image-20250424161914531.png" alt="image-20250424161914531" style="zoom:50%;" />

