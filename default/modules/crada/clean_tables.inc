<?php

function clean_tables_form($form, &$form_state) {
    $form['description'] = array(
        '#type' => 'item',
        '#title' => t('FOR DEVELOPMENT ONLY: Truncate the CRADA Database Tables and clear the existing data'),
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Clean Now',
        '#prefix' => '<div>WARNING: This will erase existing data from the crada db tables!<br/><br/>',
        '#suffix' => '</div>',

    );

    return $form;
}

function clean_tables_form_submit($form, &$form_state) {
    $result = db_truncate('crada_document')->execute();
    $result = db_truncate('crada_section')->execute();
    $result = db_truncate('crada_document_version')->execute();
    $result = db_truncate('crada_question')->execute();
    $result = db_truncate('crada_answer')->execute();
    $result = db_truncate('crada_clause_group')->execute();
    $result = db_truncate('crada_valid_clause_group_by_answer')->execute();
    $result = db_truncate('crada_document_element')->execute();
    $result = db_truncate('crada_definition')->execute();
    
    $result = db_truncate('crada_clause_group_to_clause')->execute();
	$result = db_truncate('crada_annotations')->execute();

    drupal_set_message(t('Crada Tables Have been reset to factory defaults. Good luck.'));
}