def swap(a, b):
    c = a
    a = b
    b = c
    return

class Heap:
    """Representa a una cola, con operaciones de encolar y
    desencolar. El primero en ser encolado es también el primero
    en ser desencolado."""

    def __init__(self):
        """Crea una cola vacía."""
        self.items = []

    def esta_vacia(self):
        """Devuelve True si la cola esta vacía, False si no."""
        return len(self.items) == 0

    def upheap(self, posc):
        if posc == 0:
            return
        posc_padre = (posc-1)/2
        if self.items[posc] < self.items[posc_padre]:
            return

        swap(self.items[pos], self.items[posc_padre])
        upheap(self, posc_padre)

    def downheap(self, posc_padre):
        cantidad = len(self.items)
        if posc_padre >= cantidad:
            return
        hijo_izq = (2*posc_padre)+1
        hijo_der = (2*posc_padre)+2
        posc_max = posc_padre
        if hijo_izq < cantidad and arreglo[hijo_izq] > arreglo[posc_max]:
            posc_max = hijo_izq

        if hijo_der < cantidad and arreglo[hijo_der] > arreglo[posc_max]:
            posc_max = hijo_der

        if posc_padre == posc_max:
            return

        swap(arreglo[posc_padre], arreglo[posc_max])
        downheap(self, posc_max)


    def encolar(self, x):
        """Encola el elemento x."""
        self.items.append(x)
        self.upheap(len(self.items)-1)

    def desencolar(self):
        """Elimina el primer elemento de la cola y devuelve su
        valor. Si la cola está vacía, levanta ValueError."""
        if self.esta_vacia():
            raise ValueError("La cola está vacía")
        swap(self.items[0], self.items[len(self.items)-1])
        valor = self.items.pop()
        self.downheap(0)
        return valor
