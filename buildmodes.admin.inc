<?php
// $Id$

/**
 * Lists all build modes user created
 */
function buildmodes_overview() {
  $header = array(t('Build modes'), '', t('Operations'));
  $rows = array();

  foreach (buildmodes_list_modes() as $key => $mode) {
    if ($mode['views style'] == TRUE) {
      $views_enabled = t('Views integration: <span class=\'admin-enabled\'>On</span>');
    }
    else {
      $views_enabled = t('Views integration: <span class=\'admin-disabled\'>Off</span>');
    }

    $title       = '<strong>'. $mode['title'] .'</strong>';
    $system_name = '<small class=\'description\'>'. t('System name:') .' '. $key .' <br />';
    $templates   = '<br />'. t('Template suggestion:') .' node-[content_type]-'. $key .'</small>';

    $operations  = l(t('edit'),   'admin/build/buildmodes/'. $key .'/edit') .' ';
    $operations .= l(t('delete'), 'admin/build/buildmodes/'. $key .'/delete');

    $rows[] = array(
      $title,
      $system_name . $views_enabled . $templates,
      $operations,
    );
  }

  $output = theme('table', $header, $rows, $attributes = array(), $caption = NULL);
  $output .= l('+ '. t('Add a new build mode'), 'admin/build/buildmodes/add');

  return $output;
}

/**
 * Buildmode new/edit form
 */
function buildmodes_form(&$form_state, $buildmode = NULL) {
  $form = array();

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Build mode title'),
    '#description' => t('The human readable build mode title'),
    '#default_value' => $buildmode['title'] ? $buildmode['title'] : '',
  );

  $form['name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Build mode system name'),
    '#description' => t('The machine readable build mode name. This name must contain only lowercase letters, numbers, and underscores.'),
    '#default_value' => $buildmode['name'] ? $buildmode['name'] : '',
  );

  if ($buildmode) {
    $form['old_name'] = array(
      '#type' => 'hidden',
      '#required' => TRUE,
      '#value' => $buildmode['name'],
    );
  }

  if (module_exists('views')) {
    $form['views'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Views integration'),
      '#description' => t('If this option is set you can use this build mode in Views option <em>Row style options</em> when <em>Node</em> is selected.'),
      '#options' => array(
        'views style' => t('Enable Views integration'),
      ),
      '#default_value' => array($buildmode['views style'] ? 'views style' : NULL),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['#validate'][] = 'buildmodes_validate';
  $form['#submit'][]   = 'buildmodes_submit';

  return $form;
}

function buildmodes_validate($form, &$form_state) {
  $title = trim($form_state['values']['title']);
  $name  = trim($form_state['values']['name']);

  if (!preg_match('!^[a-z0-9_]+$!', $name)) {
    form_set_error('name', t('The machine-readable name must contain only lowercase letters, numbers, and underscores.'));
  }

  $buildmodes = buildmodes_list_modes();

  if ($name == 'teaser' || $name == 'full') {
    form_set_error('name', t('The machine-readable name %name is reserved to Drupal core.', array('%name' => $name)));
  }

  if (isset($buildmodes[$name]) && !isset($form_state['values']['old_name'])) {
    form_set_error('name', t('The machine-readable name %name is already taken.', array('%name' => $name)));
  }
  elseif (isset($buildmodes[$name]) && isset($types[$form_state['values']['old_name']]) && $name != $form_state['values']['old_name']) {
    form_set_error('name', t('The machine-readable name %name is already taken.', array('%name' => $name)));
  }
}

function buildmodes_submit($form, &$form_state) {
  $buildmodes = buildmodes_list_modes();

  if (isset($form_state['values']['old_name'])) {
    unset($buildmodes[$form_state['values']['old_name']]);
  }

  $buildmodes[$form_state['values']['name']] = array(
    'title' => $form_state['values']['title'],
    'views style' => $form_state['values']['views']['views style'] === 'views style' ? TRUE : FALSE,
  );

  variable_set('buildmodes_modes', $buildmodes);

  $t_args = array('%name' => $form_state['values']['name']);
  watchdog('buildmodes', 'Build mode %name. has been saved', $t_args, WATCHDOG_NOTICE);

  drupal_set_message(t('Build mode %name has been saved.', array('%name' => $form_state['values']['title'])));
  drupal_goto('admin/build/buildmodes');
}

/**
 * Menu callback; delete a single build mode.
 */
function buildmodes_delete_confirm(&$form_state, $buildmode) {
  $form['name'] = array('#type' => 'value', '#value' => $buildmode['name']);

  $message = t('Are you sure you want to delete the build mode %name?', array('%name' => $buildmode['name']));

  $caption = '<p>'. t('This action cannot be undone.') .'</p>';

  return confirm_form($form, $message, 'admin/build/buildmodes', $caption, t('Delete'));
}

/**
 * Process build mode delete confirm submissions.
 */
function buildmodes_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    // Loads build modes
    $buildmodes = buildmodes_list_modes();

    // Unset the one we want to delete
    unset($buildmodes[$form_state['values']['name']]);

    // Save variable
    variable_set('buildmodes_modes', $buildmodes);

    $t_args = array('%name' => $form_state['values']['name']);
    drupal_set_message(t('The build mode %name has been deleted.', $t_args));
    watchdog('buildmodes', 'Deleted build mode %name.', $t_args, WATCHDOG_NOTICE);
  }
  drupal_goto('admin/build/buildmodes');
}
