<?php
/**
 * @file
 * Entity classes for Node module.
 */

/**
 * Controller class for nodes.
 *
 * This extends the DrupalDefaultEntityController class, adding required
 * special handling for node objects.
 */
class NodeController extends DrupalDefaultEntityController {

  /**
   * Overrides DrupalDefaultEntityController::attachLoad().
   */
  protected function attachLoad(&$nodes, $revision_id = FALSE) {
    // Create an array of nodes for each content type and pass this to the
    // object type specific callback.
    $typed_nodes = array();
    foreach ($nodes as $id => $entity) {
      $typed_nodes[$entity->type][$id] = $entity;
    }

    // Call object type specific callbacks on each typed array of nodes.
    foreach ($typed_nodes as $node_type => $nodes_of_type) {
      if (node_hook($node_type, 'load')) {
        $function = node_type_get_base($node_type) . '_load';
        $function($nodes_of_type);
      }
    }
    // Besides the list of nodes, pass one additional argument to
    // hook_node_load(), containing a list of node types that were loaded.
    $argument = array_keys($typed_nodes);
    $this->hookLoadArguments = array($argument);
    parent::attachLoad($nodes, $revision_id);
  }

  /**
   * Overrides DrupalDefaultEntityController::buildQuery().
   */
  protected function buildQuery($ids, $conditions = array(), $revision_id = FALSE) {
    // Ensure that uid is taken from the {node} table,
    // alias timestamp to revision_timestamp and add revision_uid.
    $query = parent::buildQuery($ids, $conditions, $revision_id);
    $fields =& $query->getFields();
    unset($fields['timestamp']);
    $query->addField('revision', 'timestamp', 'revision_timestamp');
    $fields['uid']['table'] = 'base';
    $query->addField('revision', 'uid', 'revision_uid');
    return $query;
  }
}
