**CS2023 - Algoritmos y Estructuras de Datos** 

2026-2 



# **Proyecto Final 2 - Anima tu Algoritmo o Estructura de Datos** 

_Prof. V´ıctor Racs´o Galv´an Oyola (vgalvan@utec.edu.pe)_ 

# **Anima tu Estructura de Datos** 

# **1. Descripci´on** 

En este proyecto deber´an elaborar un video educativo, al estilo de **Manim**<sup>1</sup> (Mathematical Animation Engine), que explique y demuestre visualmente el funcionamiento de una estructura de datos implementada en C/C++. El lenguaje de programaci´on y la herramienta de animaci´on son de **libre elecci´on del grupo** : no es obligatorio usar Python ni Manim espec´ıficamente. Pueden usar la combinaci´on que prefieran (por ejemplo, Manim en Python, Motion Canvas en TypeScript, Processing, o cualquier otra librer´ıa de animaci´on en el lenguaje que elijan), siempre que el resultado cumpla con el contenido m´ınimo solicitado. Se recomienda revisar los videos del canal 3Blue1Brown<sup>2</sup> como referencia de estilo e inspiraci´on. 

# **2. Estructura de datos asignada** 

A cada grupo se le asignar´a, **de forma aleatoria** , una ´unica estructura de datos seg´un lo efectivamente cubierto en clase al momento de asignar el proyecto ( **el docente confirmar´a y/o ajustar´a esta lista** seg´un corresponda); no se repetir´a una misma estructura entre grupos de una misma secci´on: 

- **BFS 0-1** : Variante de BFS que permite calcular los caminos m´ınimos desde un nodo hacia todos los dem´as en _O_ ( _|V |_ + _|E|_ ) sobre un grafo dirigido y ponderado cuyas aristas tienen peso 0 o 1. 

- **Ciclo euleriano** : Ciclo que recorre todas las aristas de un grafo exactamente una vez. Existe un algoritmo lineal para encontrar un ciclo euleriano en un grafo que lo admite. 

- **Topological sort (Tarjan)** : Algoritmo para obtener un ordenamiento topol´ogico de un grafo dirigido y ac´ıclico (DAG) en tiempo lineal, basado en DFS y en los tiempos de finalizaci´on de los nodos. 

> 1 `https://docs.manim.community/en/stable/index.html` 

> 2 `https://www.3blue1brown.com/` 

- **DSU con rollback** : Variante de DSU que permite deshacer operaciones anteriores de uni´on ( _join_ ), restaurando el estado previo de la estructura. Cada operaci´on de uni´on y su correspondiente reversi´on pueden realizarse en _O_ (1) amortizado si no se utiliza compresi´on de caminos. 

- **Virtual tree (Auxiliary tree)** : Arbol<sup>´</sup> construido a partir de un subconjunto de nodos de un ´arbol original, incluyendo los ancestros comunes necesarios para preservar las relaciones entre ellos. Se construye en _O_ ( _k_ log _k_ ) para _k_ nodos seleccionados, o en _O_ ( _k_ ) si estos ya est´an ordenados por tiempo de entrada del DFS. 

- **Centroide de un ´arbol** : Nodo cuya eliminaci´on divide el ´arbol en componentes conexas de tama˜no a lo m´as<sup>_<u>n</u>_</sup> 2<sup>.Todo´arboltienealmenosuncentroide,ysepuedeencontrarunoen</sup> tiempo _O_ ( _n_ ). 

- **Centro de un ´arbol** : Nodo que minimiza la m´axima distancia hacia cualquier otro nodo del ´arbol. Un ´arbol tiene uno o dos centros, y estos pueden encontrarse en tiempo _O_ ( _n_ ), por ejemplo, eliminando iterativamente las hojas hasta que queden uno o dos nodos. 

- **DFS Timestamps** : Tiempos de descubrimiento ( _tin_ ) y finalizaci´on ( _tout_ ) de los nodos durante un DFS. En ´arboles, permiten determinar relaciones de ancestro y verificar si un nodo pertenece al sub´arbol de otro en _O_ (1), una vez calculados mediante un DFS en _O_ ( _n_ ). 

- **Componentes biconexas** : Componentes de un grafo no dirigido que no pueden desconectarse al eliminar un ´unico v´ertice. Pueden identificarse en tiempo _O_ ( _|V |_ + _|E|_ ) mediante DFS y los tiempos de descubrimiento y valores _low_ . 

- **Algoritmo de Dial** : Algoritmo de caminos m´ınimos para grafos con pesos enteros no negativos acotados por _C_ . Utiliza una estructura de buckets para procesar las distancias y puede ejecutarse en _O_ ( _|E|_ + _C|V |_ ), mejorando la complejidad de Dijkstra cuando _C_ es suficientemente peque˜no. 

- **Topological sort (Kahn)** : Algoritmo para obtener un ordenamiento topol´ogico de un grafo dirigido y ac´ıclico (DAG) en tiempo _O_ ( _|V |_ + _|E|_ ). Utiliza los grados de entrada de los nodos y una cola para procesar primero aquellos cuyo grado de entrada es cero. 

- **Di´ametro de un ´arbol** : Camino de mayor longitud entre dos nodos del ´arbol, donde la longitud se mide mediante la cantidad de aristas o la suma de sus pesos. En un ´arbol no ponderado, puede encontrarse en tiempo _O_ ( _n_ ) mediante dos recorridos BFS o DFS. 

- **Puentes** : Aristas cuya eliminaci´on aumenta la cantidad de componentes conexas de un grafo no dirigido. Pueden encontrarse todas en tiempo _O_ ( _|V |_ + _|E|_ ) mediante DFS y los valores _low_ . 

- **DSU Tree** : Arbol construido a partir de un DSU que representa la historia de las uniones entre<sup>´</sup> componentes. Permite registrar c´omo se fusionan las componentes a lo largo del tiempo y puede utilizarse para responder consultas sobre el momento en que dos nodos pasan a pertenecer a la misma componente. Su construcci´on y utilidad dependen de la variante espec´ıfica de DSU Tree empleada. 

- **Articulaciones** : V´ertices cuya eliminaci´on aumenta la cantidad de componentes conexas de un grafo no dirigido. Pueden encontrarse todos en tiempo _O_ ( _|V |_ + _|E|_ ) mediante DFS y los valores _low_ . 

2 

- **SCC (Strongly Connected Components)** : Componentes fuertemente conexas de un grafo dirigido, es decir, conjuntos m´aximos de v´ertices tales que existe un camino dirigido entre cualquier par de v´ertices del conjunto en ambas direcciones. Pueden encontrarse todas en tiempo _O_ ( _|V |_ + _|E|_ ) mediante algoritmos como Tarjan o Kosaraju. 

- **2-SAT** : Problema de satisfacibilidad de f´ormulas booleanas en las que cada cl´ausula contiene exactamente dos literales (o, en la formulaci´on usual, a lo m´as dos). Se resuelve en tiempo _O_ ( _|V |_ + _|E|_ ) construyendo el grafo de implicaciones y calculando sus componentes fuertemente conexas. La f´ormula es satisfacible si y solo si ninguna variable y su negaci´on pertenecen a la misma componente fuertemente conexa. 

# **3. ¿Qu´e debe incluir el video?** 

Se sugiere una duraci´on de 1 a 2 minutos; en cualquier caso, **no debe exceder los 5 minutos** . La narraci´on puede ser por voz en off, texto en pantalla, o ambos. Debe contener como m´ınimo: 

- Una introducci´on conceptual: qu´e es la estructura o algoritmo, si se presenta una estructura se debe decir qu´e Tipo de Dato Abstracto (TDA) representa y para qu´e se usa en la pr´actica, si se presenta un algoritmo se debe decir para qu´e sirve y dar una breve explicaci´on de c´omo es que funciona. 

- Si se presenta una estructura de datos, se debe colocar una demostraci´on animada de sus operaciones principales; si se presenta un algoritmo, se debe colocar una demostraci´on animada de su ejecuci´on sobre una instancia ilustrativa. 

- Al menos un caso borde relevante (estructura vac´ıa, un solo elemento, peor caso, etc.). 

- Un breve an´alisis de la complejidad temporal de las operaciones mostradas o del algoritmo presentado. 

- El t´ıtulo del proyecto y los nombres de los integrantes, al inicio del video o en los cr´editos finales. 

**Nota (la animaci´on debe ser real, no simulada):** la animaci´on debe estar impulsada por una implementaci´on real de la estructura, programada por el propio grupo —no se permite animar valores o pasos “a mano” sin l´ogica real detr´as, ni usar una implementaci´on ya existente de la estructura asignada provista por alguna librer´ıa est´andar o de terceros (por ejemplo `heapq` , `std::stack` , `java.util.Stack` , si esa es justamente la estructura asignada). 

# **4. Entrega** 

- C´odigo fuente (en el lenguaje elegido) que genera la animaci´on, alojado en un **repositorio de GitHub** con un archivo `README.md` que presente el proyecto, el software requerido y los pasos para compilar/ejecutar el c´odigo. 

- Video renderizado en formato MPEG-4 ( `.mp4` ) y alta resoluci´on; si excede el l´ımite de tama˜no de Gradescope, incluyan un enlace (YouTube no listado, Drive, etc.) dentro del informe. 

- Un informe breve en PDF: car´atula (curso, integrantes, estructura asignada), descripci´on de la estructura y su TDA o algoritmo, herramientas usadas y pasos para reproducir el video, 

3 

contribuciones de cada integrante, an´alisis de complejidad, evidencia (capturas o enlace) con el enlace al repositorio de GitHub y al video, y al menos 2 conclusiones. Aseg´urense de que los enlaces funcionen correctamente. 

La entrega es grupal, ´unica, v´ıa Gradescope (solo un integrante la sube), y se califica con una r´ubrica publicada en Gradescope que eval´ua implementaci´on, animaci´on, contenido explicativo e informe. 

# **5. Modalidad y fecha l´ımite** 

Trabajan en grupos de **hasta 3 integrantes** . 

Tienen hasta el **18 de noviembre de 2026 a las 20:00** , para entregar el proyecto. 

Si se inspiran en un video o ejemplo de terceros, c´ıtenlo expl´ıcitamente en el informe. 

4 

