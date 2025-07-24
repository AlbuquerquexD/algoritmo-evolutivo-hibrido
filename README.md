# algoritmo-evolutivo-hibrido

# Problema do Percurso do Cavalo com Estratégias Evolucionárias (μ+λ)-ES

Este projeto foi desenvolvido para a disciplina de **Tópicos Avançados em Inteligência Artificial**. O objetivo é resolver o **Problema do Percurso do Cavalo (PPC)** em um tabuleiro NxN utilizando **Estratégias Evolucionárias (ES)**.

## 📌 Objetivo

Encontrar a maior sequência possível de movimentos válidos do cavalo, sem repetições de casas, em um tabuleiro de dimensão arbitrária (NxN).

## 🧬 Estratégia Evolucionária (μ+λ)-ES

A abordagem evolutiva adotada é baseada no modelo **(μ+λ)-ES**, com as seguintes características:

- **Seleção de Pais:**  
  Os **15% melhores indivíduos** (elite) da geração anterior são mantidos.

- **Geração de Filhos (λ):**  
  Para cada indivíduo da elite, **7 filhos** são gerados, totalizando λ = 7μ.

- **Torneio dos Dissimilares:**  
  Combinação do melhor pai com a pior mãe para maximizar a diversidade genética.

- **Mutação de Vizinhança:**  
  Modifica o caminho do cavalo com movimentos válidos, garantindo soluções viáveis.

- **Seleção de Sobreviventes (μ+λ):**  
  Os **melhores indivíduos entre pais e filhos** são selecionados para a próxima geração, mantendo a pressão seletiva.

## 📚 Base Teórica

Este projeto foi **inspirado em um artigo científico sobre aplicação de algoritmos evolucionários ao Problema do Percurso do Cavalo**. A implementação da Estratégia Evolucionária (ES) foi **adaptada e estendida com base nesse artigo**, respeitando os princípios teóricos, mas introduzindo Estratégia Evolucionária específicas.

## ⚙️ Tecnologias Utilizadas

- Linguagem C  
- Algoritmos Evolucionários  
- Representação baseada em permutação de casas  
- Lógica de movimento do cavalo

# 🧪 Como Reproduzir o Projeto

Este guia descreve o passo a passo para compilar e executar o projeto do **Problema do Percurso do Cavalo (PPC)** utilizando **Estratégias Evolucionárias (ES)**.

---

## ✅ Pré-requisitos

### 1. Instalar Compilador C
  - Baixe o MinGW
### Estrutura dos Arquivos
  - ppc.c → Código-fonte original (sem ES)
  - ES_PPC.c → Código com Estratégia Evolucionária (μ+λ)-ES implementada

## 📈 Resultados



## 👤 Autor

**Antony Albuquerque**  
Estudante de Bacharelado em Sistemas de Informação- UFRPE
Projeto acadêmico para Disciplina: Tópicos Avançados em Inteligência Artificial – 2025

---
