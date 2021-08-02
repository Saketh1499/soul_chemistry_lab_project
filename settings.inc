<?php
// $Id$
function chemistry_lab_settings_form($form, $form_state)
{
	$form['emails'] = array(
		'#type' => 'textfield',
		'#title' => t('(Bcc) Notification emails'),
		'#description' => t('Specify emails id for Bcc option of mail system with comma separated'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('chemistry_lab_emails', '')
	);
	$form['cc_emails'] = array(
		'#type' => 'textfield',
		'#title' => t('(Cc) Notification emails'),
		'#description' => t('Specify emails id for Cc option of mail system with comma separated'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('chemistry_lab_cc_emails', '')
	);
	$form['from_email'] = array(
		'#type' => 'textfield',
		'#title' => t('Outgoing from email address'),
		'#description' => t('Email address to be display in the from field of all outgoing messages'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('chemistry_lab_from_email', '')
	);
	$form['extensions']['abstract_upload'] = array(
		'#type' => 'textfield',
		'#title' => t('Allowed file extensions for uploading abstract file'),
		'#description' => t('A comma separated list WITHOUT SPACE of source file extensions that are permitted to be uploaded on the server'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('chemistry_lab_abstract_upload_extensions', '')
	);
	$form['extensions']['report_upload'] = array(
		'#type' => 'textfield',
		'#title' => t('Allowed file extensions for report'),
		'#description' => t('A comma separated list WITHOUT SPACE of pdf file extensions that are permitted to be uploaded on the server'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('chemistry_lab_report_upload_extensions', '')
	);
	$form['extensions']['chemistry_lab_code_upload'] = array(
		'#type' => 'textfield',
		'#title' => t('Allowed extensions for code files'),
		'#description' => t('A comma separated list WITHOUT SPACE of pdf file extensions that are permitted to be uploaded on the server'),
		'#size' => 50,
		'#maxlength' => 255,
		'#required' => TRUE,
		'#default_value' => variable_get('chemistry_lab_code_files_extensions', '')
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit')
	);
	return $form;
}
function chemistry_lab_settings_form_validate($form, &$form_state)
{
	return;
}
function chemistry_lab_settings_form_submit($form, &$form_state)
{
	variable_set('chemistry_lab_emails', $form_state['values']['emails']);
	variable_set('chemistry_lab_cc_emails', $form_state['values']['cc_emails']);
	variable_set('chemistry_lab_from_email', $form_state['values']['from_email']);
	variable_set('chemistry_lab_abstract_upload_extensions', $form_state['values']['abstract_upload']);
	variable_set('chemistry_lab_report_upload_extensions', $form_state['values']['report_upload']);
	variable_set('chemistry_lab_code_files_extensions', $form_state['values']['chemistry_lab_code_upload']);
	drupal_set_message(t('Settings updated'), 'status');
}