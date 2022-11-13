/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree) {
  *tree = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value) {
  while (tree != NULL) {
    if (key < tree->key) {
      tree = tree->left;
    }
    else if (key > tree->key) {
      tree = tree->right;
    }
    else {
      *value = tree->value;
      return true;
    }
  }
  return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value) {
  bst_node_t** current = tree;
  while (*current != NULL) {
    if (key < ( *current )->key) {
      current = &( *current )->left;
    }
    else if (key > ( *current )->key) {
      current = &( *current )->right;
    }
    else {
      // Node already exists, update value.
      (*current)->value = value;
      return;
    }
  }
  // Key doesnt exits, allocate it.
  bst_node_t* new = malloc(sizeof(bst_node_t));
  new->key = key;
  new->value = value;
  new->left = NULL;
  new->right = NULL;

  *current = new;
}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree) {
  // Find rightmost
  while ((*tree)->right != NULL) {
    tree = &( *tree )->right;
  }

  // Reassign values.
  target->key = ( *tree )->key;
  target->value = ( *tree )->value;

  // Free memory of the deleted node.
  free(*tree);
  *tree = NULL;
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key) {
  // Find element for deletion.
  while (true) {
    if ((*tree) == NULL) {
      // Key not found.
      return;
    }
    if (( *tree )->key < key) {
      tree = &(*tree)->right;
    }
    else if (( *tree )->key > key) {
      tree = &(*tree)->left;
    }
    else {
      break;
    }
  }

  // Delete the element
  // Right has value:
  if ((*tree)->left != NULL && (*tree)->right == NULL) {
    bst_node_t* delete = *tree;
    *tree = (*tree)->left;
    free(delete);
    return;
  }
  // Left has value:
  if ((*tree)->left == NULL && (*tree)->right != NULL ) {
    bst_node_t* delete = *tree;
    *tree = (*tree)->right;
    free(delete);
    return;
  }
  // Both children have value:
  if ((*tree)->left != NULL && (*tree)->right != NULL ) {
    bst_replace_by_rightmost(*tree, &(*tree)->left);
    return;
  }
  // Neither child exists:
  else {
    bst_node_t* delete = *tree;
    *tree = NULL;
    free(delete);
    return;
  }

}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree) {
  bst_node_t* current = *tree;
  stack_bst_t stack;
  stack_bst_init(&stack);

  bool dont_exit = true;
  // If a node is on the stack, it means that i freed left but not right.
  // Fill stack with branch
  do {
    while (current != NULL) {
      stack_bst_push(&stack, current);
      current = current->right;
    }

    current = stack.items[stack.top]->left;
    free(stack_bst_top(&stack));

    if (stack_bst_empty(&stack)) {
      dont_exit = false;
    }
    else {
      stack_bst_pop(&stack);
    }
  } while (!stack_bst_empty(&stack) || dont_exit);

  *tree = NULL;
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit) {
  while (tree != NULL) {
    stack_bst_push(to_visit, tree);
    bst_print_node(tree);
    tree = tree->left;
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree) {
  stack_bst_t to_visit;
  stack_bst_init(&to_visit);

  bst_leftmost_preorder(tree, &to_visit);
  while (!stack_bst_empty(&to_visit)) {
    tree = stack_bst_pop(&to_visit);
    bst_leftmost_preorder(tree->right, &to_visit);
  }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit) {
  while(tree != NULL) {
    stack_bst_push(to_visit, tree);
    tree = tree->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree) {
  stack_bst_t to_visit;
  stack_bst_init(&to_visit);

  bst_leftmost_inorder(tree, &to_visit);
  while (!stack_bst_empty(&to_visit)) {
    tree = stack_bst_top(&to_visit);
    stack_bst_pop(&to_visit);
    bst_print_node(tree);
    bst_leftmost_inorder(tree->right, &to_visit);
  }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit) {
  while (tree != NULL) {
    stack_bst_push(to_visit, tree);
    stack_bool_push(first_visit, tree);
    tree = tree->left;
  }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree) {
  bool from_left;
  stack_bst_t to_visit;
  stack_bool_t first_visit;
  stack_bst_init(&to_visit);
  stack_bool_init(&first_visit);

  bst_leftmost_postorder(tree, &to_visit, &first_visit);
  while (!stack_bst_empty(&to_visit)) {
    tree = stack_bst_top(&to_visit);
    from_left = stack_bool_pop(&first_visit);
    if (from_left) {
      stack_bool_push(&first_visit, false);
      bst_leftmost_postorder(tree->right, &to_visit, &first_visit);
    }
    else {
      stack_bst_pop(&to_visit);
      bst_print_node(tree);
    }
  }
}
