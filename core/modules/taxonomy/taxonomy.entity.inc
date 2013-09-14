<?php
/**
 * @file
 * Entity classes for Taxonomy module.
 */

/**
 * Controller class for taxonomy terms.
 *
 * This extends the DrupalDefaultEntityController class. Only alteration is
 * that we match the condition on term name case-independently.
 */
class TaxonomyTermController extends DrupalDefaultEntityController {

  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions, $revision_id);
    $query->addTag('translatable');
    $query->addTag('term_access');
    // When name is passed as a condition use LIKE.
    if (isset($conditions['name'])) {
      $query_conditions = &$query->conditions();
      foreach ($query_conditions as $key => $condition) {
        if ($condition['field'] == 'base.name') {
          $query_conditions[$key]['operator'] = 'LIKE';
          $query_conditions[$key]['value'] = db_like($query_conditions[$key]['value']);
        }
      }
    }
    // Add the machine name field from the {taxonomy_vocabulary} table.
    $query->innerJoin('taxonomy_vocabulary', 'v', 'base.vid = v.vid');
    $query->addField('v', 'machine_name', 'vocabulary_machine_name');
    return $query;
  }

  protected function cacheGet($ids, $conditions = array()) {
    $terms = parent::cacheGet($ids, $conditions);
    // Name matching is case insensitive, note that with some collations
    // LOWER() and drupal_strtolower() may return different results.
    foreach ($terms as $term) {
      $term_values = (array) $term;
      if (isset($conditions['name']) && drupal_strtolower($conditions['name'] != drupal_strtolower($term_values['name']))) {
        unset($terms[$term->tid]);
      }
    }
    return $terms;
  }
}

/**
 * Controller class for taxonomy vocabularies.
 *
 * This extends the DrupalDefaultEntityController class, adding required
 * special handling for taxonomy vocabulary objects.
 */
class TaxonomyVocabularyController extends DrupalDefaultEntityController {

  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    $query = parent::buildQuery($ids, $conditions, $revision_id);
    $query->addTag('translatable');
    $query->orderBy('base.weight');
    $query->orderBy('base.name');
    return $query;
  }
}
