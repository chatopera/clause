<?php
/**
 * Autogenerated by Thrift Compiler (0.12.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
use Thrift\Base\TBase;
use Thrift\Type\TType;
use Thrift\Type\TMessageType;
use Thrift\Exception\TException;
use Thrift\Exception\TProtocolException;
use Thrift\Protocol\TProtocol;
use Thrift\Protocol\TBinaryProtocolAccelerated;
use Thrift\Exception\TApplicationException;

/**
 * 命名实体标识
 */
class Entity
{
    static public $isValidate = false;

    static public $_TSPEC = array(
        1 => array(
            'var' => 'name',
            'isRequired' => false,
            'type' => TType::STRING,
        ),
        2 => array(
            'var' => 'val',
            'isRequired' => false,
            'type' => TType::STRING,
        ),
        3 => array(
            'var' => 'requires',
            'isRequired' => false,
            'type' => TType::BOOL,
        ),
        4 => array(
            'var' => 'dictname',
            'isRequired' => false,
            'type' => TType::STRING,
        ),
    );

    /**
     * @var string
     */
    public $name = null;
    /**
     * @var string
     */
    public $val = null;
    /**
     * @var bool
     */
    public $requires = null;
    /**
     * @var string
     */
    public $dictname = null;

    public function __construct($vals = null)
    {
        if (is_array($vals)) {
            if (isset($vals['name'])) {
                $this->name = $vals['name'];
            }
            if (isset($vals['val'])) {
                $this->val = $vals['val'];
            }
            if (isset($vals['requires'])) {
                $this->requires = $vals['requires'];
            }
            if (isset($vals['dictname'])) {
                $this->dictname = $vals['dictname'];
            }
        }
    }

    public function getName()
    {
        return 'Entity';
    }


    public function read($input)
    {
        $xfer = 0;
        $fname = null;
        $ftype = 0;
        $fid = 0;
        $xfer += $input->readStructBegin($fname);
        while (true) {
            $xfer += $input->readFieldBegin($fname, $ftype, $fid);
            if ($ftype == TType::STOP) {
                break;
            }
            switch ($fid) {
                case 1:
                    if ($ftype == TType::STRING) {
                        $xfer += $input->readString($this->name);
                    } else {
                        $xfer += $input->skip($ftype);
                    }
                    break;
                case 2:
                    if ($ftype == TType::STRING) {
                        $xfer += $input->readString($this->val);
                    } else {
                        $xfer += $input->skip($ftype);
                    }
                    break;
                case 3:
                    if ($ftype == TType::BOOL) {
                        $xfer += $input->readBool($this->requires);
                    } else {
                        $xfer += $input->skip($ftype);
                    }
                    break;
                case 4:
                    if ($ftype == TType::STRING) {
                        $xfer += $input->readString($this->dictname);
                    } else {
                        $xfer += $input->skip($ftype);
                    }
                    break;
                default:
                    $xfer += $input->skip($ftype);
                    break;
            }
            $xfer += $input->readFieldEnd();
        }
        $xfer += $input->readStructEnd();
        return $xfer;
    }

    public function write($output)
    {
        $xfer = 0;
        $xfer += $output->writeStructBegin('Entity');
        if ($this->name !== null) {
            $xfer += $output->writeFieldBegin('name', TType::STRING, 1);
            $xfer += $output->writeString($this->name);
            $xfer += $output->writeFieldEnd();
        }
        if ($this->val !== null) {
            $xfer += $output->writeFieldBegin('val', TType::STRING, 2);
            $xfer += $output->writeString($this->val);
            $xfer += $output->writeFieldEnd();
        }
        if ($this->requires !== null) {
            $xfer += $output->writeFieldBegin('requires', TType::BOOL, 3);
            $xfer += $output->writeBool($this->requires);
            $xfer += $output->writeFieldEnd();
        }
        if ($this->dictname !== null) {
            $xfer += $output->writeFieldBegin('dictname', TType::STRING, 4);
            $xfer += $output->writeString($this->dictname);
            $xfer += $output->writeFieldEnd();
        }
        $xfer += $output->writeFieldStop();
        $xfer += $output->writeStructEnd();
        return $xfer;
    }
}
