arquivo = open('arquivo-saida.txt', 'r')
erro = False
for linha in arquivo:
    linha = linha.strip()
    print("linha=",linha)
    numeros = linha.split(" ")
    # numeros = []
    # for i,num in enumerate(l):
    #    # print(num[i])
    #     x = num[i]
    #     numeros.append(x)
    # print(numeros)
    for index, num in enumerate(numeros, start=0):
        
        if numeros[index-1] >= numeros[index]:
    #        print(numeros[index-1] ," >= ",numeros[index], "index=", index)
            erro = True
if(erro):
    print("Erro no arquivo :(")
else: 
    print("Tudo certo no arquivo :)")


arquivo.close()
