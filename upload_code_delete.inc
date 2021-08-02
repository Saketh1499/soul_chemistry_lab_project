<?php

/******************************************************************************/
/***************************** DELETE CODE ************************************/
/******************************************************************************/

function soul_chemistry_lab_upload_code_delete()
{
  global $user;

  $root_path = chemistry_lab_path();
  $solution_id = (int)arg(3);

  /* check solution */
 // $solution_q = db_query("SELECT * FROM {soul_chemistry_lab_solution} WHERE id = %d LIMIT 1", $solution_id);
  $query = db_select('soul_chemistry_lab_solution');
              $query->fields('soul_chemistry_lab_solution');
              $query->condition('id', $solution_id);
              $query->range(0, 1);
              $solution_q = $query->execute();
  $solution_data = $solution_q->fetchObject();
  if (!$solution_data)
  {
    drupal_set_message('Invalid solution.', 'error');
    drupal_goto('chemistry-lab-project/code');
    return;
  }
  if ($solution_data->approval_status != 0)
  {
    drupal_set_message('You cannnot delete a solution after it has been approved. Please contact site administrator if you want to delete this solution.', 'error');
    drupal_goto('chemistry-lab-project/code');
    return;
  }

  //$experiment_q = db_query("SELECT * FROM {soul_chemistry_lab_experiment} WHERE id = %d LIMIT 1", $solution_data->experiment_id);
  $query = db_select('soul_chemistry_lab_experiment');
            $query->fields('soul_chemistry_lab_experiment');
            $query->condition('id', $solution_data->experiment_id);
            $query->range(0, 1);
            $experiment_q = $query->execute();

  $experiment_data = $experiment_q->fetchObject();
  if (!$experiment_data)
  {
    drupal_set_message('You do not have permission to delete this solution.', 'error');
    drupal_goto('chemistry-lab-project/code');
    return;
  }

  //$proposal_q = db_query("SELECT * FROM {soul_chemistry_lab_proposal} WHERE id = %d AND solution_provider_uid = %d LIMIT 1", $experiment_data->proposal_id, $user->uid);
  $query = db_select('soul_chemistry_lab_proposal');
              $query->fields('soul_chemistry_lab_proposal');
              $query->condition('id', $experiment_data->proposal_id);
              $query->condition('solution_provider_uid', $user->uid);
              $query->range(0, 1);
              $proposal_q = $query->execute();
  $proposal_data = $proposal_q->fetchObject();
  if (!$proposal_data)
  {
    drupal_set_message('You do not have permission to delete this solution.', 'error');
    drupal_goto('chemistry-lab-project/code');
    return;
  }

  /* deleting solution files */
  if (soul_chemistry_lab_delete_solution($solution_data->id))
  {
    drupal_set_message('Solution deleted.', 'status');

    /* sending email */
    $email_to = $user->mail;

    $from = variable_get('soul_chemistry_lab_from_email', '');
    $bcc= variable_get('soul_chemistry_lab_emails', '');
    $cc=variable_get('soul_chemistry_lab_cc_emails', '');  

    $param['solution_deleted_user']['lab_title'] = $proposal_data->lab_title;
    $param['solution_deleted_user']['experiment_title'] = $experiment_data->title;
    $param['solution_deleted_user']['solution_number'] = $solution_data->code_number;
    $param['solution_deleted_user']['solution_caption'] = $solution_data->caption;
    $param['solution_deleted_user']['user_id'] = $user->uid;
    $param['solution_deleted_user']['headers']=array('From'=>$from,'MIME-Version'=> '1.0',
    			'Content-Type'=> 'text/plain; charset=UTF-8; format=flowed; delsp=yes',
    			'Content-Transfer-Encoding' => '8Bit',
    			'X-Mailer'=> 'Drupal','Cc' => $cc, 'Bcc' => $bcc);

    if (!drupal_mail('soul_chemistry_lab', 'solution_deleted_user', $email_to, language_default(), $param , $from , TRUE))
      drupal_set_message('Error sending email message.', 'error');
  } else {
    drupal_set_message('Error deleting example.', 'status');
  }

  drupal_goto('chemistry-lab-project/code');
  return;
}