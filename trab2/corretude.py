arquivo = open('arquivo-saida.txt', 'r')
erro = False
for linha in arquivo:
    linha = linha.strip()
    #print("linha=",linha)
    numeros = linha.split(" ")
    # numeros = []
    # for i,num in enumerate(l):
    #    # print(num[i])
    #     x = num[i]
    #     numeros.append(x)
    # print(numeros)
    #for index, num in enumerate(numeros, start=0):
    x = len(numeros)
    index=1
    while(index<x):
        num1 = int(numeros[index-1].strip())
        num2 = int(numeros[index].strip())
        if num1 > num2:
            print(numeros[index-1] ,">=",numeros[index], "index=", index)
            erro = True
        index = index + 1
if(erro):
    print("Erro no arquivo :(")
else: 
    print("Tudo certo no arquivo :)")


arquivo.close()
