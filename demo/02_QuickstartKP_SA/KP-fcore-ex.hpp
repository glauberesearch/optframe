// OptFrame Demo 0-1 Knapsack Problem + Simulated Annealing
// File: KP-fcore-ex.hpp
#pragma once

#include <OptFCore/FCore.hpp>
#include <iostream>
#include <vector>

#include <algorithm>
#include <functional>

#include <OptFrame/Core.hpp>
#include <OptFrame/Heuristics/Heuristics.hpp> // many metaheuristics here...
#include <OptFrame/Scanner++/Scanner.hpp>
#include <OptFrame/Util/Matrix.hpp>
#include <OptFrame/printable/printable.hpp>

using namespace std;
using namespace optframe;
using namespace scannerpp;

// namespace para o problema da mochila
// namespace KP_fcore {

// Solução para o problema da mochila, e elemento de avaliação

// ================================
//    Definição do 'ESolution'
// Par: (Representação, Avaliação)
// ================================

using ESolutionKP = std::pair<
  std::vector<bool>, // (representation)
  Evaluation<int>    // (objective value)
  >;
class ProblemContext
{
public:
   int n;         // numero de itens
   int Q;         // capacidade máxima da mochila
   vector<int> p; // lucro 'p' de cada item
   vector<int> w; // peso 'w' de cada item
   //
   // leitor de dados "estilo Java" (Scanner)
   void load(Scanner& scanner)
   {
      n = *scanner.nextInt(); // leitura do numero de itens na mochila
      Q = *scanner.nextInt(); // leitura da capacidade da mochila
      p = vector<int>(n);     // realoca espaço
      w = vector<int>(n);     // realoca espaço
      //
      cout << "n=" << n << " Q=" << Q << endl;
      //
      // leitura do lucro do item i
      for (int i = 0; i < n; i++) {
         p[i] = *scanner.nextInt(); // '*' é usado pq saída do 'nextInt' é 'optional'
         cout << "p[" << i << "]=" << p[i] << " ";
      }
      cout << endl;
      //
      // leitura do peso do item i
      for (int i = 0; i < n; i++) {
         w[i] = *scanner.nextInt(); // '*' é usado pq saída do 'nextInt' é 'optional'
         cout << "w[" << i << "]=" << w[i] << " ";
      }
      cout << endl;
   }
};
// Instanciar um problema da mochila pKP
ProblemContext pKP;

std::vector<bool>
frandom()
{
   vector<bool> v(pKP.n, false); // começa sem nenhum item escolhido
   for (unsigned i = 0; i < v.size(); i++)
      v[i] = rand() % 2; // sorteia se leva item ou não (resultado 0 ou 1)
   // retorna solução
   return v;
}

// Gerador de solução inicial (método construtivo)
FConstructive<std::vector<bool>> randomConstructive{ frandom };

// ===========================
//    Função de avaliação
// ===========================

// função de avaliação da mochila
Evaluation<int>
fevaluate(const std::vector<bool>& s)
{
   int f = 0;
   int somaPeso = 0;
   for (int i = 0; i < pKP.n; i++)
      if (s[i]) // se elemento foi escolhido
      {
         f += pKP.p[i];        // acumula lucro do elemento i
         somaPeso += pKP.w[i]; // acumula peso do elemento i
      }
   // verifica capacidade excedida
   if (somaPeso >= pKP.Q)
      f -= 1000000 * (somaPeso - pKP.Q); // punição proporcional ao excesso de peso
   //
   return Evaluation<int>{ f };
}

// Evaluate (false -> maximização)
FEvaluator<ESolutionKP, MAXIMIZE> evalKP{ fevaluate };
//
class MoveBitFlip : public Move<ESolutionKP>
{
public:
   int k; // MoveBitFlip (moveData = 'int' (k))

   MoveBitFlip(int _k)
     : k{ _k }
   {
   }

   uptr<Move<ESolutionKP>> apply(ESolutionKP& se) override
   {
      se.first[k] = !se.first[k];                           // reverts element 'k'
      return uptr<Move<ESolutionKP>>(new MoveBitFlip{ k }); // returns reverse move
   }
};

uptr<Move<ESolutionKP>>
makeMoveBitFlip(int k)
{
   return uptr<Move<ESolutionKP>>(new MoveBitFlip{ k });
}
// gerador de movimentos aleatórios do tipo BitFlip
uptr<Move<ESolutionKP>>
fRandomFlip(const ESolutionKP& se)
{
   int k = ::rand() % pKP.n; // sorteia um item entre [0..n-1]
   // cria um "movimento" de flip, na posição k, com operação 'fApplyFlip'
   return uptr<Move<ESolutionKP>>(makeMoveBitFlip(k));
}

// Estrutura de Vizinhança para BitFlip (NS)
FNS<ESolutionKP> nsFlip{ fRandomFlip };
