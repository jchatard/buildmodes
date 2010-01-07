<?php

function buildmodes_overview() {
  $header = array(t('Build modes'), t('Operations'));
  $rows = array();

  //foreach (variable_get('buildmodes_modes', array()) as $key => $value) {
  foreach (buildmodes_list_modes() as $key => $mode) {
    if ($mode['views style'] == TRUE) {
      $views_enabled = t('Views integration is <span class=\'admin-enabled\'>On</span>');
    }
    else {
      $views_enabled = t('Views integration is <span class=\'admin-disabled\'>Off</span>');
    }
    $rows[] = array(
      "<strong>". $mode['title'] ."</strong><br /><small>System name: $key <br />$views_enabled</small>",
      l(t('edit'), 'admin/build/buildmodes/edit/'. $key),
    );
  }

  $output = theme('table', $header, $rows, $attributes = array(), $caption = NULL);
  $output .= l('+ '. t('Add a new build mode'), 'admin/build/buildmodes/new');
  
  return $output;
}

function buildmodes_new_form() {
  $form = array();

  $form['title'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Build mode title'),
    '#description' => t('The human readable build mode title'),
  );
  
  $form['name'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Build mode system name'),
    '#description' => t('The machine readable build mode name. This name must contain only lowercase letters, numbers, and underscores.'),
  );
  
  if (module_exists('views')) {
    $form['views'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Views integration'),
      '#description' => t('If this option is set you can use this build mode in Views option <em>Row style options</em> when <em>Node</em> is selected.'),
      '#options' => array(
        'views style' => t('Enable Views integration'),
      ),
    );
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  $form['#validate'][] = 'buildmodes_new_validate';
  $form['#submit'][]   = 'buildmodes_new_submit';

  return $form;
}

function buildmodes_new_validate($form, &$form_state) {
  $title = trim($form_state['values']['title']);
  $name  = trim($form_state['values']['name']);

  if (!preg_match('!^[a-z0-9_]+$!', $name)) {
    form_set_error('name', t('The machine-readable name must contain only lowercase letters, numbers, and underscores.'));
  }
  
  $types = buildmodes_list_modes();

  if (isset($types[$name])) {
    form_set_error('name', t('The machine-readable name %name is already taken.', array('%name' => $name)));
  }
  
}

function buildmodes_new_submit($form, &$form_state) {
  $buildmodes = buildmodes_list_modes();

  $buildmodes[$form_state['values']['name']] = array(
    'title' => $form_state['values']['title'],
    'views style' => $form_state['values']['views']['views style'] === 'views style' ? TRUE : FALSE,
  );

  variable_set('buildmodes_modes', $buildmodes);
  drupal_set_message(t('New build mode %name has been created.', array('%name' => $form_state['values']['title'])));
  drupal_goto('admin/build/buildmodes');
}