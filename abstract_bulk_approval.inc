<?php

function chemistry_lab_abstract_bulk_approval_form($form, &$form_state)
{
	$options_first = _bulk_list_of_chemistry_lab_project();
	$selected = isset($form_state['values']['chemistry_lab_project']) ? $form_state['values']['chemistry_lab_project'] : key($options_first);
	$form = array();
	$form['chemistry_lab_project'] = array(
		'#type' => 'select',
		'#title' => t('Title of the chemistry lab project'),
		'#options' => _bulk_list_of_chemistry_lab_project(),
		'#default_value' => $selected,
		'#ajax' => array(
			
			'callback' => 'ajax_bulk_chemistry_lab_abstract_details_callback'
		),
		'#suffix' => '<div id="ajax_selected_chemistry_lab"></div><div id="ajax_selected_chemistry_lab_pdf"></div>'
	);
	$form['chemistry_lab_actions'] = array(
		'#type' => 'select',
		'#title' => t('Please select action for chemistry lab project'),
		'#options' => _bulk_list_chemistry_lab_actions(),
		'#default_value' => 0,
		'#prefix' => '<div id="ajax_selected_chemistry_lab_action" style="color:red;">',
		'#suffix' => '</div>',
		'#states' => array(
			'invisible' => array(
				':input[name="chemistry_lab_project"]' => array(
					'value' => 0
				)
			)
		)
	);
	$form['message'] = array(
		'#type' => 'textarea',
		'#title' => t('Please specify the reason for Resubmission / Dis-Approval'),
		'#prefix' => '<div id= "message_submit">',
		'#states' => array(
			'visible' => array(
				array(
					':input[name="chemistry_lab_actions"]' => array(
						'value' => 3
					)
				),
				'or',
				array(
					':input[name="chemistry_lab_actions"]' => array(
						'value' => 2
					)
				)
			)
		)
	);
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Submit'),
		'#states' => array(
			'invisible' => array(
				':input[name="lab"]' => array(
					'value' => 0
				)
			)
		)
	);
	return $form;
}
function ajax_bulk_chemistry_lab_abstract_details_callback($form, $form_state)
{
	$commands = array();
	$chemistry_lab_project_default_value = $form_state['values']['chemistry_lab_project'];
	if ($chemistry_lab_project_default_value != 0)
	{
		$commands[] = ajax_command_html('#ajax_selected_chemistry_lab', _chemistry_lab_details($chemistry_lab_project_default_value));
		$form['chemistry_lab_actions']['#options'] = _bulk_list_chemistry_lab_actions();
		$commands[] = ajax_command_replace('#ajax_selected_chemistry_lab_action', drupal_render($form['chemistry_lab_actions']));
	} //$chemistry_lab_project_default_value != 0
	else
	{
		$commands[] = ajax_command_html('#ajax_selected_chemistry_lab', '');
		$commands[] = ajax_command_data('#ajax_selected_chemistry_lab', 'form_state_value_select', $form_state['values']['chemistry_lab_project']);
	}
	return array(
		'#type' => 'ajax',
		'#commands' => $commands
	);
}
/************************************************************/
function chemistry_lab_abstract_bulk_approval_form_submit($form, &$form_state)
{
	global $user;
	$msg = '';
	$root_path = chemistry_lab_document_path();
	if ($form_state['clicked_button']['#value'] == 'Submit')
	{
		if ($form_state['values']['chemistry_lab_project'])
		// chemistry_lab_abstract_del_lab_pdf($form_state['values']['chemistry_lab_project']);
			if (user_access('soul chemistry lab bulk manage abstract'))
			{
				$query = db_select('soul_chemistry_lab_proposal');
				$query->fields('soul_chemistry_lab_proposal');
				$query->condition('id', $form_state['values']['chemistry_lab_project']);
				$user_query = $query->execute();
				$user_info = $user_query->fetchObject();
				$user_data = user_load($user_info->uid);
				if ($form_state['values']['chemistry_lab_actions'] == 1)
				{
					// approving entire project //
					$query = db_select('soul_chemistry_lab_submitted_abstracts');
					$query->fields('soul_chemistry_lab_submitted_abstracts');
					$query->condition('proposal_id', $form_state['values']['chemistry_lab_project']);
					$abstracts_q = $query->execute();
					$experiment_list = '';
					while ($abstract_data = $abstracts_q->fetchObject())
					{
						db_query("UPDATE {soul_chemistry_lab_submitted_abstracts} SET abstract_approval_status = 1, is_submitted = 1, approver_uid = :approver_uid WHERE id = :id", array(
							':approver_uid' => $user->uid,
							':id' => $abstract_data->id
						));
						db_query("UPDATE {soul_chemistry_lab_submitted_abstracts_file} SET file_approval_status = 1, approvar_uid = :approver_uid WHERE submitted_abstract_id = :submitted_abstract_id", array(
							':approver_uid' => $user->uid,
							':submitted_abstract_id' => $abstract_data->id
						));
					} //$abstract_data = $abstracts_q->fetchObject()
					drupal_set_message(t('Approved chemistry lab project.'), 'status');
					// email 
					$email_subject = t('[!site_name][chemistry lab Project] Your uploaded chemistry lab project have been approved', array(
						'!site_name' => variable_get('site_name', '')
					));
					$email_body = array(
						0 => t('

Dear !user_name,

Your uploaded abstract for the chemistry lab project has been approved:

Title of chemistry lab project  : ' . $user_info->project_title . '

Best Wishes,

!site_name Team,
FOSSEE,IIT Bombay', array(
							'!site_name' => variable_get('site_name', ''),
							'!user_name' => $user_data->name
						))
					);
					/** sending email when everything done **/
					$email_to = $user_data->mail;
					$from = variable_get('chemistry_lab_from_email', '');
					$bcc = variable_get('chemistry_lab_emails', '');
					$cc = variable_get('chemistry_lab_cc_emails', '');
					$params['standard']['subject'] = $email_subject;
					$params['standard']['body'] = $email_body;
					$params['standard']['headers'] = array(
						'From' => $from,
						'MIME-Version' => '1.0',
						'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
						'Content-Transfer-Encoding' => '8Bit',
						'X-Mailer' => 'Drupal',
						'Cc' => $cc,
						'Bcc' => $bcc
					);
					if (!drupal_mail('chemistry_lab', 'standard', $email_to, language_default(), $params, $from, TRUE))
					{
						$msg = drupal_set_message('Error sending email message.', 'error');
					} //!drupal_mail('chemistry_lab', 'standard', $email_to, language_default(), $params, $from, TRUE)
				} //$form_state['values']['chemistry_lab_actions'] == 1
				elseif ($form_state['values']['chemistry_lab_actions'] == 2)
				{
					if (strlen(trim($form_state['values']['message'])) <= 30)
					{
						form_set_error('message', t(''));
						$msg = drupal_set_message("Please mention the reason for resubmission. Minimum 30 character required", 'error');
						return $msg;
					}
					//pending review entire project 
					$query = db_select('soul_chemistry_lab_submitted_abstracts');
					$query->fields('soul_chemistry_lab_submitted_abstracts');
					$query->condition('proposal_id', $form_state['values']['chemistry_lab_project']);
					$abstracts_q = $query->execute();
					$experiment_list = '';
					while ($abstract_data = $abstracts_q->fetchObject())
					{
						db_query("UPDATE {soul_chemistry_lab_submitted_abstracts} SET abstract_approval_status = 0, is_submitted = 0, approver_uid = :approver_uid WHERE id = :id", array(
							':approver_uid' => $user->uid,
							':id' => $abstract_data->id
						));
						db_query("UPDATE {soul_chemistry_lab_proposal} SET is_submitted = 0, approver_uid = :approver_uid WHERE id = :id", array(
							':approver_uid' => $user->uid,
							':id' => $abstract_data->proposal_id
						));
						db_query("UPDATE {soul_chemistry_lab_submitted_abstracts_file} SET file_approval_status = 0, approvar_uid = :approver_uid WHERE submitted_abstract_id = :submitted_abstract_id", array(
							':approver_uid' => $user->uid,
							':submitted_abstract_id' => $abstract_data->id
						));
					} //$abstract_data = $abstracts_q->fetchObject()
					drupal_set_message(t('Resubmit the project files'), 'status');
					// email 
					$email_subject = t('[!site_name][chemistry lab Project] Your uploaded chemistry lab project have been marked as pending', array(
						'!site_name' => variable_get('site_name', '')
					));
					$email_body = array(
						0 => t('

Dear !user_name,

Kindly resubmit the project files for the project : ' . $user_info->project_title . 'after making changes considering the following reviewer’s comments.

Comment: ' . $form_state['values']['message'] . '

Best Wishes,

!site_name Team,
FOSSEE, IIT Bombay', array(
							'!site_name' => variable_get('site_name', ''),
							'!user_name' => $user_data->name
						))
					);
					/** sending email when everything done **/
					$email_to = $user_data->mail;
					$from = variable_get('chemistry_lab_from_email', '');
					$bcc = variable_get('chemistry_lab_emails', '');
					$cc = variable_get('chemistry_lab_cc_emails', '');
					$params['standard']['subject'] = $email_subject;
					$params['standard']['body'] = $email_body;
					$params['standard']['headers'] = array(
						'From' => $from,
						'MIME-Version' => '1.0',
						'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
						'Content-Transfer-Encoding' => '8Bit',
						'X-Mailer' => 'Drupal',
						'Cc' => $cc,
						'Bcc' => $bcc
					);
					if (!drupal_mail('chemistry_lab', 'standard', $email_to, language_default(), $params, $from, TRUE))
					{
						drupal_set_message('Error sending email message.', 'error');
					} //!drupal_mail('chemistry_lab', 'standard', $email_to, language_default(), $params, $from, TRUE)
				} //$form_state['values']['chemistry_lab_actions'] == 2
				elseif ($form_state['values']['chemistry_lab_actions'] == 3) //disapprove and delete entire chemistry lab project
				{
					if (strlen(trim($form_state['values']['message'])) <= 30)
					{
						form_set_error('message', t(''));
						$msg = drupal_set_message("Please mention the reason for disapproval. Minimum 30 character required", 'error');
						return $msg;
					} //strlen(trim($form_state['values']['message'])) <= 30
					if (!user_access('soul chemistry lab bulk delete abstract'))
					{
						$msg = drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Lab.'), 'error');
						return $msg;
					} //!user_access('chemistry_lab bulk delete code')
					if (chemistry_lab_abstract_delete_project($form_state['values']['chemistry_lab_project'])) //////
					{
						drupal_set_message(t('Dis-Approved and Deleted Entire chemistry lab project.'), 'status');
						$email_subject = t('[!site_name][chemistry lab Project] Your uploaded chemistry lab project have been marked as dis-approved', array(
						'!site_name' => variable_get('site_name', '')
					));
					$email_body = array(
						0 => t('

Dear !user_name,

Your uploaded chemistry lab project files for the chemistry lab project Title : ' . $user_info->project_title . ' have been marked as dis-approved.

Reason for dis-approval: ' . $form_state['values']['message'] . '

Best Wishes,

!site_name Team,
FOSSEE, IIT Bombay', array(
					'!site_name' => variable_get('site_name', ''),
					'!user_name' => $user_data->name
											))
					);
					$email_to = $user_data->mail;
					$from = variable_get('chemistry_lab_from_email', '');
					$bcc = variable_get('chemistry_lab_emails', '');
					$cc = variable_get('chemistry_lab_cc_emails', '');
					$params['standard']['subject'] = $email_subject;
					$params['standard']['body'] = $email_body;
					$params['standard']['headers'] = array(
						'From' => $from,
						'MIME-Version' => '1.0',
						'Content-Type' => 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
						'Content-Transfer-Encoding' => '8Bit',
						'X-Mailer' => 'Drupal',
						'Cc' => $cc,
						'Bcc' => $bcc
					);
					if (!drupal_mail('chemistry_lab', 'standard', $email_to, language_default(), $params, $from, TRUE))
					{
						drupal_set_message('Error sending email message.', 'error');
					}
					} //chemistry_lab_abstract_delete_project($form_state['values']['chemistry_lab_project'])
					else
					{
						drupal_set_message(t('Error Dis-Approving and Deleting Entire chemistry lab project.'), 'error');
					}
					// email 
					
				} //$form_state['values']['chemistry_lab_actions'] == 3
				/*elseif ($form_state['values']['chemistry_lab_actions'] == 4)
				{
					if (strlen(trim($form_state['values']['message'])) <= 30)
					{
						form_set_error('message', t(''));
						$msg = drupal_set_message("Please mention the reason for disapproval/deletion. Minimum 30 character required", 'error');
						return $msg;
					} //strlen(trim($form_state['values']['message'])) <= 30
					$query = db_select('soul_chemistry_lab_abstract_experiment');
					$query->fields('soul_chemistry_lab_abstract_experiment');
					$query->condition('proposal_id', $form_state['values']['lab']);
					$query->orderBy('number', 'ASC');
					$experiment_q = $query->execute();
					$experiment_list = '';
					while ($experiment_data = $experiment_q->fetchObject())
					{
						$experiment_list .= '<p>' . $experiment_data->number . ') ' . $experiment_data->title . '<br> Description :  ' . $experiment_data->description . '<br>';
						$experiment_list .= ' ';
						$experiment_list .= '</p>';
					} //$experiment_data = $experiment_q->fetchObject()
					if (!user_access('lab migration bulk delete code'))
					{
						$msg = drupal_set_message(t('You do not have permission to Bulk Delete Entire Lab Including Proposal.'), 'error');
						return $msg;
					} //!user_access('lab migration bulk delete code')
					// check if dependency files are present 
					$dep_q = db_query("SELECT * FROM {soul_chemistry_lab_abstract_dependency_files} WHERE proposal_id = :proposal_id", array(
						":proposal_id" => $form_state['values']['lab']
					));
					if ($dep_data = $dep_q->fetchObject())
					{
						$msg = drupal_set_message(t("Cannot delete lab since it has dependency files that can be used by others. First delete the dependency files before deleting the lab."), 'error');
						return $msg ;
					} //$dep_data = $dep_q->fetchObject()
					if (chemistry_lab_abstract_delete_lab($form_state['values']['lab']))
					{
						drupal_set_message(t('Dis-Approved and Deleted Entire Lab solutions.'), 'status');
						$query = db_select('soul_chemistry_lab_abstract_experiment');
						$query->fields('soul_chemistry_lab_abstract_experiment');
						$query->condition('proposal_id', $form_state['values']['lab']);
						$experiment_q = $query->execute()->fetchObject();
						$dir_path = $root_path . $experiment_q->directory_name;
						if (is_dir($dir_path))
						{
							$res = rmdir($dir_path);
							if (!$res)
							{
								$msg = drupal_set_message(t("Cannot delete Lab directory : " . $dir_path . ". Please contact administrator."), 'error');
								return $msg;
							} //!$res
						} //is_dir($dir_path)
						else
						{
							drupal_set_message(t("Lab directory not present : " . $dir_path . ". Skipping deleting lab directory."), 'status');
						}
						$proposal_q = db_query("SELECT * FROM {soul_chemistry_lab_abstract_proposal} WHERE id = :id", array(
							":id" => $form_state['values']['lab']
						));
						$proposal_data = $proposal_q->fetchObject();
						$proposal_id = $proposal_data->id;
						db_query("DELETE FROM {soul_chemistry_lab_abstract_experiment} WHERE proposal_id = :proposal_id", array(
							":proposal_id" => $proposal_id
						));
						db_query("DELETE FROM {soul_chemistry_lab_abstract_proposal} WHERE id = :id", array(
							":id" => $proposal_id
						));
						drupal_set_message(t('Deleted Lab Proposal.'), 'status');
						//email 
						$email_subject = t('[!site_name] Your uploaded Lab Migration solutions including the Lab proposal have been deleted', array(
							'!site_name' => variable_get('site_name', '')
						));
						$email_body = array(
							0 => t('

Dear !user_name,

We regret to inform you that all the uploaded Experiments of your Lab with following details have been deleted permanently.

Title of Lab :' . $user_info->lab_title . '

List of experiments : ' . $experiment_list . '

Reason for dis-approval: ' . $form_state['values']['message'] . '

Best Wishes,

!site_name Team,
FOSSEE,IIT Bombay', array(
								'!site_name' => variable_get('site_name', ''),
								'!user_name' => $user_data->name
							))
						);
						// email 
						//  $email_subject = t('Your uploaded Lab Migration solutions including the Lab proposal have been deleted');
						$email_body = array(
							0 => t('Your all the uploaded solutions including the Lab proposal have been deleted permanently.')
						);
					} //chemistry_lab_abstract_delete_lab($form_state['values']['lab'])
					else
					{
						$msg = drupal_set_message(t('Error Dis-Approving and Deleting Entire Lab.'), 'error');
					}
				} //$form_state['values']['chemistry_lab_actions'] == 4
				else
				{
					$msg = drupal_set_message(t('You do not have permission to bulk manage code.'), 'error');
				}*/
			} //user_access('chemistry_lab project bulk manage code')
		return $msg;
	} //$form_state['clicked_button']['#value'] == 'Submit'
}
/**********************************************************/
function _bulk_list_of_chemistry_lab_project()
{
	$project_titles = array(
		'0' => 'Please select...'
	);
	$query = db_select('soul_chemistry_lab_proposal');
	$query->fields('soul_chemistry_lab_proposal');
	$query->condition('is_submitted', 1);
	$query->condition('approval_status', 1);
	$query->orderBy('project_title', 'ASC');
	$project_titles_q = $query->execute();
	while ($project_titles_data = $project_titles_q->fetchObject())
	{
		$project_titles[$project_titles_data->id] = $project_titles_data->project_title . ' (Proposed by ' . $project_titles_data->contributor_name . ')';
	} //$project_titles_data = $project_titles_q->fetchObject()
	return $project_titles;
}
function _bulk_list_chemistry_lab_actions()
{
	$chemistry_lab_actions = array(
		0 => 'Please select...'
	);
	$chemistry_lab_actions[1] = 'Approve Entire chemistry lab Project';
	$chemistry_lab_actions[2] = 'Resubmit Project files';
	$chemistry_lab_actions[3] = 'Dis-Approve Entire chemistry lab Project (This will delete chemistry lab Project)';
	//$chemistry_lab_actions[4] = 'Delete Entire chemistry lab Project Including Proposal';
	return $chemistry_lab_actions;
}
function _chemistry_lab_details($chemistry_lab_proposal_id)
{
	$return_html = "";
	$query_pro = db_select('soul_chemistry_lab_proposal');
	$query_pro->fields('soul_chemistry_lab_proposal');
	$query_pro->condition('id', $chemistry_lab_proposal_id);
	$abstracts_pro = $query_pro->execute()->fetchObject();
	//var_dump($abstracts_pro);die;
	$query_pdf = db_select('soul_chemistry_lab_submitted_abstracts_file');
	$query_pdf->fields('soul_chemistry_lab_submitted_abstracts_file');
	$query_pdf->condition('proposal_id', $chemistry_lab_proposal_id);
	$query_pdf->condition('filetype', 'A');
	$abstracts_pdf = $query_pdf->execute()->fetchObject();
	if ($abstracts_pdf == TRUE)
	{
		if ($abstracts_pdf->filename != "NULL" || $abstracts_pdf->filename != "")
		{
			$abstract_filename = $abstracts_pdf->filename;
		} //$abstracts_pdf->filename != "NULL" || $abstracts_pdf->filename != ""
		else
		{
			$abstract_filename = "File not uploaded";
		}
	} //$abstracts_pdf == TRUE
	else
	{
		$abstract_filename = "File not uploaded";
	}
	$query_process = db_select('soul_chemistry_lab_submitted_abstracts_file');
	$query_process->fields('soul_chemistry_lab_submitted_abstracts_file');
	$query_process->condition('proposal_id', $chemistry_lab_proposal_id);
	$query_process->condition('filetype', 'S');
	$abstracts_query_process = $query_process->execute()->fetchObject();
	$query = db_select('soul_chemistry_lab_submitted_abstracts');
	$query->fields('soul_chemistry_lab_submitted_abstracts');
	$query->condition('proposal_id', $chemistry_lab_proposal_id);
	$abstracts_q = $query->execute()->fetchObject();
	if ($abstracts_q)
	{
		if ($abstracts_q->is_submitted == 0)
		{
			//drupal_set_message(t('Abstract is not submmited yet.'), 'error', $repeat = FALSE);
			//return;
		} //$abstracts_q->is_submitted == 0
	} //$abstracts_q
	//var_dump($abstracts_query_process);die;
	if ($abstracts_query_process == TRUE)
	{
		if ($abstracts_query_process->filename != "NULL" || $abstracts_query_process->filename != "")
		{
			$abstracts_query_process_filename = $abstracts_query_process->filename;
		} //$abstracts_query_process->filename != "NULL" || $abstracts_query_process->filename != ""
		else
		{
			$abstracts_query_process_filename = "File not uploaded";
		}
	} //$abstracts_query_process == TRUE
	else
	{
		$url = l('Upload abstract', 'chemistry-lab-project/abstract-code/upload');
		$abstracts_query_process_filename = "File not uploaded";
	}
	$download_chemistry_lab = l('Download chemistry lab project','chemistry-lab-project/full-download/project/'.$chemistry_lab_proposal_id);
	$return_html .= '<strong>Proposer Name:</strong><br />' . $abstracts_pro->name_title . ' ' . $abstracts_pro->contributor_name . '<br /><br />';
	$return_html .= '<strong>Title of the chemistry lab Project:</strong><br />' . $abstracts_pro->project_title . '<br /><br />';
	$return_html .= '<strong>Uploaded an abstract (brief outline) of the project:</strong><br />' . $abstract_filename . '<br /><br />';
	$return_html .= '<strong>Upload the soul chemistry lab for the developed process:</strong><br />' . $abstracts_query_process_filename . '<br /><br />';
	$return_html .= $download_chemistry_lab;
	return $return_html;
}