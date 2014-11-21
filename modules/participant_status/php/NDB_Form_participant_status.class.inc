<?php
/**
 * This form contains the code for the participant status module of Loris.
 * The module allows you to save the status of a candidate (ie. 
 * active/withdrawn/etc) on a candidate level. The valid options for statuses
 * are in the table participant_status_options, as well as whether or not
 * that status requires a reason to be specified.
 *
 * PHP Version 5
 *
 *  @category Behavioural 
 *  @package  Main
 *  @author   Zia Mohadesz <zia.mohades@gmail.com>
 *  @license  Loris License
 *  @link     https://github.com/aces/Loris-Trunk
 */
require_once "NDB_Form.class.inc";
require_once "Utility.class.inc";


class NDB_Form_participant_status extends NDB_Form
{
    var $indent = "&nbsp;&nbsp;&nbsp;&nbsp;";
    var $multipleSelect = array('excluded_from_trial');

    /**
     * Determine if the user should have access to this page.
     * Currently tied to data_entry permission.
     *
     * @return boolean True if the user should have access
     */
    function _hasAccess()
    {
        // create user object
        $user =& User::singleton();
        if (Utility::isErrorX($user)) {
            return PEAR::raiseError("User Error: ".$user->getMessage());
        }

        // check that the $candID is passed b/c the profile needs to be opened
        if ($this->identifier != $_REQUEST['candID']) {
            return PEAR::raiseError(
                "NDB_Form_participant_status::_access: " 
                . "Candidate Profile Error ($this->identifier)"
            );
        }
        // check user permissions
        return ($user->hasPermission('data_entry'));
    }

    /**
     * Gets default options for smarty to fill in.
     * This comes from the participant status table
     *
     * @return array Currently saved entries in database
     */
    function _getDefaults()
    {

        $DB =& Database::singleton();
        $defaults = $DB->pselectRow(
            "SELECT * FROM participant_status WHERE CandID = :cid", 
            array('cid'=>$this->identifier)
        );

        if(!empty($this->multipleSelect)){
            foreach($this->multipleSelect AS $elname){
                if(stristr($defaults[$elname], "{@}")){
                    $defaults[$elname]=explode("{@}",$defaults[$elname]);
                }
            }
        }
/*
        $result = $DB->pselect("SELECT participant_status, entry_staff, data_entry_date, data_change_date FROM participant_status_history
                                WHERE CandID=:cid", array('cid'=>$this->identifier));
        for($i=0; $i < sizeof($result); $i++) {
            $row = $result[$i];
            $desc = $DB->pselectOne("SELECT Description FROM participant_status_options WHERE ID=:pid",
                                     array('pid'=>$row['participant_status']));
            $this->participant_history["status".$i] = $desc;
            $this->participant_history["staff".$i]  = $row['entry_staff'];
            $this->participant_history["timestamp".$i] = $row['data_entry_date'];
            $this->participant_history[“datachanged”.$i] = $row['data_changed_date'];
        }
*/

            //clearing this date because don't want to accidentally reuse last saved date
            $defaults['data_changed_date'] = null;
        return $defaults;
    }
    /**
     * Process the submitted values, called after validation.
     *
     * @param array $values The values submitted by the user 
     *                      to the form.
     *
     * @return boolean True on success
     */
    function _process($values)
    {
        if (!is_array($values) || count($values) === 0) {
            return true;
        }
        $DB =& Database::singleton();

        $values['CandID'] = $this->identifier;

        if (!(is_null($_SESSION['State']))) {
            $currentUser =& User::singleton($_SESSION['State']->getUsername());
            $id = $currentUser->getData("UserID");
            $values['entry_staff'] = $id;
        }

        //Convert select multiple elements into database storable values
        if(!empty($this->multipleSelect)){
            foreach($this->multipleSelect AS $elname){
                if(isset($values[$elname]) && is_array($values[$elname])){
                    $values[$elname]=implode("{@}",$values[$elname]);
                }
            }
        }

//don't want to allow for non-null dates....	
//        if ($values['data_changed_date']['Y'] == "" && $values['data_changed_date']['M'] == "" && $values['data_changed_date']['d'] == "") {
//                $values['data_changed_date'] = NULL;
//        } else {
                $values['data_changed_date'] = sprintf("%04d-%02d-%02d", $values['data_changed_date']['Y'], $values['data_changed_date']['M'], $values['data_changed_date']['d']);
//        }
	if ($values['naproxen_eligibility_status'] == 'death' || $values['probucol_eligibility_status'] == 'death') {
		$values['participant_status'] = 6;
	}


        // save the values
        //if it exists update it
        $ParticipantCount = $DB->pselectOne(
            "SELECT COUNT(*) FROM participant_status WHERE CandID = :cid", 
            array("cid"=>$values['CandID'])
        );
        if ($ParticipantCount > 0) {
            $success = $DB->update(
                "participant_status", 
                $values,
                array("CandID"=>$values['CandID'])
            );
        } else {
            //insert it
            $success = $DB->insert("participant_status", $values);
        }

        if (Utility::isErrorX($success)) {
            return PEAR::raiseError(
                "NDB_Form_participant_status::_process: ".$success->getMessage()
            );
        }

	// update participant_status_history
        $success = $DB->insert("participant_status_history", $values);

        if (Utility::isErrorX($success)) {
            return PEAR::raiseError(
                "NDB_Form_participant_status::_process: ".$success->getMessage()
            );
        }

        $this->tpl_data['success'] = true;
        $this->form->freeze();
    }

    /**
     * Constructor called by loris to setup the tpl_data
     * array
     *
     * @return null
     */
    function participant_status()
    {

        $this->tpl_data['candID'] = $this->identifier;

        $config =& NDB_Config::singleton();
        $DB =& Database::singleton();

        //entry staff info
        $this->addScoreColumn('entry_staff', 'Data Entry Staff');
        $this->addScoreColumn('data_entry_date', 'Date of last Data entry');

        $pscid = $DB->pselectone(
            'SELECT PSCID FROM candidate where CandID = :Candid',
            array('Candid'=>$this->identifier)
        );
        $this->addScoreColumn('pscid', 'PSCID');
        $this->tpl_data['pscid'] = $pscid;

        // Selections array (standard template for questions selections)
        $notAns4 = array(
//            "dnk"            => "Don't know", 
//            "not_applicable" => "Not applicable",
////            "refusal"        => "Refusal",
            "not_answered"   => "Not Answered"
        );

        $this->participant_status_history();
        $options = $this->getParticipantStatusOptions();

        $this->addSelect(
            'participant_status', 
            "Participant Status", 
            array('') + $options //+ $notAns4
        );
//old
        $this->addTextAreaGroup(
            'reason_specify', 
            $this->indent . 'If status is not Active 1 or Withdrawn, please specify reason:',
            array(null => null) + $notAns4
        );
//new
        $this->addTextAreaGroup(
            'reason_specify',
            'If status is Ineligible or Excluded, please specify reason:',
            array(null => null) + $notAns4
        );

        $subprojectOptions = $config->getSetting('subprojects');
        foreach(Utility::toArray($subprojectOptions['subproject']) AS $subproject){
            if ($subproject['id'] != 1)
                $subprojects[$subproject['id']]=$subproject['title'];
        }

        $this->addSelect(
            'excluded_from_trial',
            $this->indent . "If excluded from trial, specify which trial(s)",
            $subprojects, 
            "multiple size = '3'"
        );


	$eligibilityYesNo = array("null"=>"","yes"=>"Yes","no"=>"No");
	$eligibilityStatus = array('null'=>'','active'=>'Active','stop_medication_active'=>'Stop Medication Active','withdrawn'=>'Withdrawn','excluded'=>'Excluded','death'=>'Death','completed'=>'Completed','stop_medication_completed'=>'Stop Medication Completed');
        $this->addSelect(
            'naproxen_eligibility',
            "Eligible for Naproxen trial?",
            $eligibilityYesNo
        );
        $this->addTextAreaGroup(
            'naproxen_eligibility_reason_specify',
            'If not eligible for Naproxen and Active(Global Status), please specify reason:',
            array(null => null) + $notAns4
        );
        $this->addSelect(
            'naproxen_eligibility_status',
            "If eligible for Naproxen, what is the Naproxen status?",
            $eligibilityStatus
        );
        $this->addTextAreaGroup(
            'naproxen_excluded_reason_specify',
            'If Naproxen Status is Excluded or Stop Medication Active, please specify reason:',
            array(null => null) + $notAns4
        );

        $this->addSelect(
            'probucol_eligibility',
            "Eligible for Probucol trial?",
            $eligibilityYesNo
        );
        $this->addTextAreaGroup(
            'probucol_eligibility_reason_specify',
            'If not eligible for Probucol and Active(Global Status), please specify reason:',
            array(null => null) + $notAns4
        );
        $this->addSelect(
            'probucol_eligibility_status',
            "If eligible for Probucol, what is the Probucol status?",
            $eligibilityStatus
        );
        $this->addTextAreaGroup(
            'probucol_excluded_reason_specify',
            'If Probucol Status is Excluded or Stop Medication Active, please specify reason:',
            array(null => null) + $notAns4
        );

        $config =& NDB_Config::singleton();
        $dateOptions = array(
        'language' => 'en',
        'format'   => 'YMd',
        'minYear'  => $config->getSetting('startYear'),
        'maxYear'  => $config->getSetting('endYear'),
        'addEmptyOption' => true,
        'emptyOptionValue' => null
		);
	$this->addBasicDate("data_changed_date", "Date of Status Change:", $dateOptions);

   
        //drop out
        $options = array(
            null                     => '', 
            "1_voluntary_withdrawal" => "1. Voluntary withdrawal", 
            "2_recommended_withdrawal" => "2. Recommended withdrawal (e.g. by doctor)", 
            "3_lost_follow_up"       => "3. Lost to follow up", 
            "4_other"               => "4. Other"
        );
//old
        $this->addSelect(
            'withdrawal_reasons',
            "If status is Withdrawn, please specify:",
            $options
        );
//new
        $this->addSelect(
            'withdrawal_reasons',
            "If status is Withdrawn, please specify:",
            $options
        );

        $this->addTextAreaGroup(
            'withdrawal_reasons_other_specify', 
            $this->indent . 'If other reason for withdrawal, please specify:',
            array(null => null) + $notAns4
        );

        $this->addSelect(
            'naproxen_withdrawal_reasons',
            "If Naproxen Status is Withdrawn, please specify:",
            $options
        );
        $this->addTextAreaGroup(
            'naproxen_withdrawal_reasons_other_specify',
            $this->indent . 'If reason specified for withdrawal, please explain:',
            array(null => null) + $notAns4
        );
        $this->addSelect(
            'probucol_withdrawal_reasons',
            "If Probucol Status is Withdrawn, please specify:",
            $options
        );
        $this->addTextAreaGroup(
            'probucol_withdrawal_reasons_other_specify',
            $this->indent . 'If reason specified for withdrawal, please explain:',
            array(null => null) + $notAns4
        );


        $this->form->addFormRule(array(&$this,'_validate'));
    }


    function participant_status_history() {
        
        $this->tpl_data['candID'] = $_REQUEST['candID'];
        $this->addHeader('Paricipant Status History for Candidate');
        $DB =& Database::singleton();
        $pscid = $DB->pselectone(
                'SELECT PSCID FROM candidate where CandID = :candid',
                array('candid'=>$this->identifier)
                );

        $this->tpl_data['pscid'] = $pscid;

        $result = $DB->pselect("SELECT po.Description,
                  naproxen_eligibility,naproxen_eligibility_reason_specify,naproxen_eligibility_status,
                  probucol_eligibility,probucol_eligibility_reason_specify,probucol_eligibility_status,
                  entry_staff, data_changed_date, data_entry_date 
                  FROM participant_status_history p join participant_status_options po on (po.ID = p.participant_status)
                  WHERE CandID=:cid", array('cid'=>$this->identifier));
        for($i = 0; $i< sizeof($result); $i++) {
            if($result[$i]['Description']=='Active' && empty($result[$i]['entry_staff']))
            {
                $result[$i]['entry_staff'] = 'Enrollment';
            }    
            
        }  
        $this->tpl_data['history_list'] = $result;
    }

    /** 
     * This function is for validation of user input, depending on
     * the questions answered.
     *
     * @param array $fields The list of input to the form submitted by
     *                      smarty.
     *
     * @return array An array of elements with errors. An empty array if
     *               all elements are valid.
     */
    function _validate($fields)
    {
        $errors=array();
        $options = $this->getParticipantStatusOptionsVerbose();

        $reasonID = $fields['participant_status'];


        if ($options[$reasonID]['Required'] == true) {

            if (($options[$reasonID]['Description'])=='Withdrawn') {
                if (empty($fields['withdrawal_reasons'])) {
                    $errors['withdrawal_reasons'] = "required";
                } else if ($fields['withdrawal_reasons'] == '3_other') {
                    if (empty($fields['withdrawal_reasons_other_specify'])
                        && empty($fields['withdrawal_reasons_other_specify_status'])
                    ) {
                        $errors['withdrawal_reasons_other_specify_group']
                            = "required";
                    }
                }
            } else {
                if (empty($fields['reason_specify'])
                    && empty($fields['reason_specify_status'])
                ) {
                    $errors['reason_specify_group'] = "Must specify when status is "
                        . $options[$reasonID]['Description'];
                }
            }
        }

        // Not withdrawal

        if ((!empty($fields['withdrawal_reasons'])) 
            && (($options[$reasonID]['Description'])!='Withdrawn')
        ) {
            $errors['withdrawal_reasons'] = 'Only specify if withdrawn';
        }

        // if global ineligible or excluded, reason must be specified
        if (($fields['participant_status'] == 3 || $fields['participant_status'] == 4) && (empty($fields['reason_specify']) && empty($fields['reason_specify_status']))) {
            $errors['reason_specify_group'] = 'Required if Global Status is Ineligible or Excluded';
        }

	// if eligible for naproxen, must specify status
        if ($fields['naproxen_eligibility'] == "yes" && $fields['naproxen_eligibility_status'] == 'null') {
            $errors['naproxen_eligibility_status'] = "Required if eligible for Naproxen";
	}

        // if not eligible for naproxen and global status is active
        if ($fields['naproxen_eligibility'] == "no" && $fields['participant_status'] == 1 && empty($fields['naproxen_eligibility_reason_specify'])) {
            $errors['naproxen_eligibility_reason_specify_group'] = "Required if not eligible but Global Status is Active";
        }

        // if Naproxen excluded or stop medication active, reason required
        if (($fields['naproxen_eligibility_status'] == 'excluded' || $fields['naproxen_eligibility_status'] == 'stop_medication_active') && (empty($fields['naproxen_excluded_reason_specify']) && empty($fields['naproxen_excluded_reason_specify_status']))) {
            $errors['naproxen_excluded_reason_specify_group'] = "Required if Naproxen Excluded or Stop Medication Active";
        }

        // if Naproxen withdrawn, reason required
        if ($fields['naproxen_eligibility_status'] == 'withdrawn' && empty($fields['naproxen_withdrawal_reasons'])) {
            $errors['naproxen_withdrawal_reasons'] = "Required if Naproxen Withdrawn";
        }

        // if Naproxen withdrawn, and reason is required, then the 'other' reason is required as a further explanation 
        if (($fields['naproxen_eligibility_status'] == 'withdrawn' && !empty($fields['naproxen_withdrawal_reasons'])) && (empty($fields['naproxen_withdrawal_reasons_other_specify']) && empty($fields['naproxen_withdrawal_reasons_other_specify_status']))) {
            $errors['naproxen_withdrawal_reasons_other_specify_group'] = "Required";
        }

	// if eligible for probucol, must specify status
        if ($fields['probucol_eligibility'] == "yes" && $fields['probucol_eligibility_status'] == 'null') {
            $errors['probucol_eligibility_status'] = "Required if eligible for Probucol";
	}

        // if not eligible for probucol and global status is active
        if ($fields['probucol_eligibility'] == "no" && $fields['participant_status'] == 1 && empty($fields['probucol_eligibility_reason_specify'])) {
            $errors['probucol_eligibility_reason_specify_group'] = "Required if not eligible but Global Status is Active";
        }

        // if probucol excluded or stop medication active, reason required
        if (($fields['probucol_eligibility_status'] == 'excluded' || $fields['probucol_eligibility_status'] == 'stop_medication_active') && (empty($fields['probucol_excluded_reason_specify']) && empty($fields['probucol_excluded_reason_specify_status']))) {
            $errors['probucol_excluded_reason_specify_group'] = "Required if Probucol Excluded or Stop Medication Active";
        }

        // if Probucol withdrawn, reason required
        if ($fields['probucol_eligibility_status'] == 'withdrawn' && empty($fields['probucol_withdrawal_reasons'])) {
            $errors['probucol_withdrawal_reasons'] = "Required if Probucol Withdrawn";
        }

        // if Probucol withdrawn, and reason is required, then the 'other' reason is required as a further explanation
        if (($fields['probucol_eligibility_status'] == 'withdrawn' && !empty($fields['probucol_withdrawal_reasons'])) && (empty($fields['probucol_withdrawal_reasons_other_specify']) && empty($fields['probucol_withdrawal_reasons_other_specify_status']))) {
            $errors['probucol_withdrawal_reasons_other_specify_group'] = "Required";
        }

        // if Global Active, must specify if Naproxen or Probucol eligibility
        if ($fields['participant_status'] == 1 && ($fields['naproxen_eligibility'] == 'null' && $fields['probucol_eligibility'] == 'null')) {
            $errors['participant_status'] = "Please specify at least 1 trial eligibility below";
        }

	// date of status change required
	if ($fields['data_changed_date']['Y'] == "" || $fields['data_changed_date']['M'] == "" || $fields['data_changed_date']['d'] == "") {
            $errors['data_changed_date'] = 'Required';
	}
        return $errors;
    }

    /**
     * Gets the participant_status options from participant_status_options
     * getParticipantStatusOptions()
     *
     * @return array Options array suitable for use in QuickForm select
     *               element
     */
    function getParticipantStatusOptions()
    {
        $DB =& Database::singleton();
        $options = $DB->pselect(
            "SELECT ID,Description FROM participant_status_options WHERE parentID IS NULL",
            array()
        );
        $option_array = array();
        foreach ($options as $option) {
            $option_array[$option['ID']] = $option['Description'];
        }
        return $option_array;
    }

    /**
     * Get the participant_status options as an array, where each row
     * is an array
     *
     * @return array An array of options, where each row is an array containing
     *               the entire row from participant_status_options instead of
     *               only the description
     */
    function getParticipantStatusOptionsVerbose()
    {
        $DB =& Database::singleton();
        $options = $DB->pselect(
            "SELECT * FROM participant_status_options",
            array()
        );

        $option_array = array();
        foreach ($options as $option) {
            $ID = $option['ID'];
            unset($option['ID']);
            $option_array[$ID] = $option;
        }
        return $option_array;
    }
}
?>
