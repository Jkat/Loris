<?php
/**
 * This class provides the code to create the menu filter form that allows
 * for data entry of entities that should be acknowledged in publications
 *
 * PHP Version 5
 *
 * @category Behavioural
 * @package  Main
 * @author   Justin Kat <justin.kat@mail.mcgill.ca>
 * @license  http://www.gnu.org/licenses/gpl-3.0.txt GPLv3
 * @link     https://github.com/aces/Loris
 */
namespace LORIS\acknowledgements;

/**
 * This class provides the code to create the menu filter form that allows
 * for data entry of entities that should be acknowledged in publications
 *
 * PHP Version 5
 *
 * @category Behavioural
 * @package  Main
 * @author   Justin Kat <justin.kat@mail.mcgill.ca>
 * @license  http://www.gnu.org/licenses/gpl-3.0.txt GPLv3
 * @link     https://github.com/aces/Loris
 */

class Acknowledgements extends \NDB_Menu_Filter_Form
{

    /**
     * _has_access returns true
     * if the user has the specific permission
     *
     * @return boolean
     */
    function _hasAccess()
    {
        // create user object and check appropriate permission
        $user =& \User::singleton();
        return ($user->hasPermission('acknowledgements_view'));
    }

    /**
     * Processes the submitted form
     *
     * @param array $values Array of submitted values
     *
     * @return boolean
     */
    function _process($values)
    {
        // create user object and check appropriate permission
        $user =& \User::singleton();
        if (!$user->hasPermission('acknowledgements_edit')) {
            return;
        }

        if (!is_array($values) || count($values) == 0) {
            return true;
        }

        $results = array();
        foreach ($values as $key => $value) {
            $results[substr($key, 3)] = $value;
        }
        $values = $results;

        $count = 1;
        foreach ($values['affiliations'] as &$value) {
            $value = $count . ")" . $value;
            $count++;
        }
        unset($value);
        $values['affiliations'] = implode(" ", $values['affiliations']);

        $values['degrees']      = implode(",", $values['degrees']);
        $values['roles']        = implode(",", $values['roles']);

        $DB =& \Database::singleton();
        if (isset($_POST['fire_away'])) {

            if (empty($values['present'])) {
                $values['present'] = null;
            }

            if (empty($values['end_date'])) {
                $values['end_date'] = null;
            }

            if (empty($values['start_date'])) {
                $values['start_date'] = null;
            }

            if ($values['present'] == 'Yes') {
                $values['end_date'] = null;
            }

            $DB->insert('acknowledgements', $values);
            unset($values);
        }

        $this->tpl_data['success'] = true;
    }

    /**
     * Sets up all the class variables needed for the
     * acknowledgements menu filter
     *
     * @return true on success
     */
    function _setupVariables()
    {
        $config = \NDB_Config::singleton();

        $this->columns      = array(
                               'first_name',
                               'initials',
                               'last_name',
                               'affiliations',
                               'degrees',
                               'roles',
                               'start_date',
                               'end_date',
                               'present',
                              );
        $this->query        = " FROM acknowledgements WHERE 1=1";
        $this->validFilters = array(
                               'first_name',
                               'last_name',
                               'start_date',
                               'end_date',
                               'present',
                              );
        $this->headers      = array(
                               'first_name',
                               'initials',
                               'last_name',
                               'affiliations',
                               'degrees',
                               'roles',
                               'start_date',
                               'end_date',
                               'present',
                              );
        $this->formToFilter = array(
                               'first_name' => 'first_name',
                               'last_name'  => 'last_name',
                               'start_date' => 'start_date',
                               'end_date'   => 'end_date',
                               'present'    => 'present',
                              );

        $this->tpl_data['citation_policy'] = $config->getSetting('citation_policy');

        return true;
    }

    /**
     * Does the setup required for this page. By default, sets up elements
     * that are common to every type of page. May be overridden by a specific
     * page or specific page type.
     *
     * @return none
     */
    function setup()
    {
        parent::setup();

        // enums should match with acknowledgements table
        $present = array(
                    ''    => '',
                    'Yes' => 'Yes',
                    'No'  => 'No',
                   );

        // add filter elements
        $this->addBasicText('first_name', 'First Name:');
        $this->addBasicText('last_name', 'Last Name:');
        $config      =& \NDB_Config::singleton();
        $startYear   = $config->getSetting('startYear');
        $endYear     = $config->getSetting('endYear');
        $dateOptions = array(
                        'language'       => 'en',
                        'format'         => 'YMd',
                        'addEmptyOption' => true,
                        'minYear'        => $startYear,
                        'maxYear'        => $endYear,
                       );
        $this->addBasicDate('start_date', 'Start Date:', $dateOptions);
        $this->addBasicDate('end_date', 'End Date:', $dateOptions);
        $this->addSelect('present', 'Present?', $present);

        // add form elements
        $this->addBasicText('addfirst_name', 'First Name:');
        $this->addBasicText('addinitials', 'Initials:');
        $this->addBasicText('addlast_name', 'Last Name:');
        $config      =& \NDB_Config::singleton();
        $startYear   = $config->getSetting('startYear');
        $endYear     = $config->getSetting('endYear');
        $dateOptions = array(
                        'language'       => 'en',
                        'format'         => 'YMd',
                        'addEmptyOption' => true,
                        'minYear'        => $startYear,
                        'maxYear'        => $endYear,
                       );
        $this->addBasicDate('addstart_date', 'Start Date:', $dateOptions);
        $this->addBasicDate('addend_date', 'End Date:', $dateOptions);
        $this->addSelect('addpresent', 'Present?', $present);

        return true;
    }

    /**
     * Add dependency on default acknowledgements.js
     *
     * @return array of javascript files to be included
     */
    function getJSDependencies()
    {
        $factory = \NDB_Factory::singleton();
        $baseurl = $factory->settings()->getBaseURL();

        $baseDeps = parent::getJSDependencies();

        return array_merge(
            $baseDeps,
            array(
             $baseurl . '/acknowledgements/js/acknowledgements_helper.js',
            )
        );

    }

    function _getList()
    {
        // create DB object
        $DB =& \Database::singleton();

        $qparams = array();
        // add the base query
        $query  = '';
        $query .= $this->_getBaseQuery();

        $filterdetails = $this->_getBaseFilter();
        $query        .= $filterdetails['clause'];
        $qparams       = $filterdetails['params'];
        // apply ORDER BY filters
        $query .= " ORDER BY ";
        if (!empty($this->filter['order'])) {
            $query .= $this->filter['order']['field']
                ." ".$this->filter['order']['fieldOrder'].", ";
        }
        $query .= $this->order_by;
        // get the list

        $pageNum = 1;
        if (isset($_REQUEST['pageID'])) {
            $pageNum = $_REQUEST['pageID'];
        }

        $this->_getNumberPages($query);

        $config         =& \NDB_Config::singleton();
//        $resultsPerPage = $config->getSetting('rowsPerPage');
//        $limit          = " LIMIT $resultsPerPage"
//                          ." OFFSET " . (($pageNum-1)*$resultsPerPage);
        $result         = $DB->pselect($query . $limit, $qparams);

        return $result;
    }

}

?>

