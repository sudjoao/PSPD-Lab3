# PSPD-Lab3

# Pré requisitos
- [mpich](https://www.mpich.org)
- [gcc](https://gcc.gnu.org)
## Como rodar
Para compilar o software basta executar o comando:
```
make
```

E após isso executar o comando para rodar
```
mpirun -n *numero_de_workers* bin/bin.out
```
Onde o *numero_de_workers* deve ser substituído pela quantidade desejada.

Além disso, você pode alterar o arquivo file.txt para o texto desejado.
