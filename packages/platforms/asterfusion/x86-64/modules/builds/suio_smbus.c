#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/i2c.h>

enum kinds { nct6106, nct6775, nct6776, nct6779, nct6791, nct6792, nct6793,
    nct6795, nct6796, nct6797, nct6798 };

/* used to set data->name = nct6775_device_names[data->sio_kind] */
static const char * const nct6775_device_names[] = {
    "nct6106",
    "nct6775",
    "nct6776",
    "nct6779",
    "nct6791",
    "nct6792",
    "nct6793",
    "nct6795",
    "nct6796",
    "nct6797",
    "nct6798",
};

static const char * const nct6775_sio_names[] = {
    "NCT6106D",
    "NCT6775F",
    "NCT6776D/F",
    "NCT6779D",
    "NCT6791D",
    "NCT6792D",
    "NCT6793D",
    "NCT6795D",
    "NCT6796D",
    "NCT6797D",
    "NCT6798D",
};


#define SM_DATA 0
#define SM_WRITE_SIZE 1
#define SM_COMMAND 2
#define SM_INDEX 3
#define SM_CONTROL 4
#define SM_ADDRESS 5
#define SM_FERQ 6
#define BS_MASK	(~7)
#define ERROR_STATUS 9
#define SMCTL3 0xE
#define CHIP_ID 58

/*
 * Super-I/O constants and functions
 */

#define NCT6775_LD_ACPI		0x0a
#define NCT6775_LD_HWM		0x0b
#define NCT6775_LD_VID		0x0d
#define NCT6775_LD_12		0x12

#define SIO_REG_LDSEL		0x07	/* Logical device select */
#define SIO_REG_DEVID		0x20	/* Device ID (2 bytes) */
#define SIO_REG_ENABLE		0x30	/* Logical device enable */
#define SIO_REG_ADDR		0x60	/* Logical device address (2 bytes) */

#define SIO_ADDR            0x2e
#define SIO_NCT6106_ID		0xc450
#define SIO_NCT6775_ID		0xb470
#define SIO_NCT6776_ID		0xc330
#define SIO_NCT6779_ID		0xc560
#define SIO_NCT6791_ID		0xc800
#define SIO_NCT6792_ID		0xc910
#define SIO_NCT6793_ID		0xd120
#define SIO_NCT6795_ID		0xd350
#define SIO_NCT6796_ID		0xd420
#define SIO_NCT6797_ID		0xd450
#define SIO_NCT6798_ID		0xd428
#define SIO_ID_MASK		0xFFF8

#define DRVNAME "sio_smbus"
int smbus_clock = 2;  //0x2=0010=200KHz=100KHz of waveform
module_param(smbus_clock, int, 0);

struct sio_smbus_priv {
    struct i2c_adapter adapter;
    struct i2c_msg *msg;
    struct device *dev;
    struct mutex acpi_lock;
    int bs_addr;
};

    static inline int
superio_inb(int ioreg, int reg)
{
    outb(reg, ioreg);
    return inb(ioreg + 1);
}

	static inline void
superio_outb(int ioreg, int reg, int val)
{
	outb(reg, ioreg);
	outb(val, ioreg + 1);
}

    static inline int
superio_enter(int ioreg)
{
    if (!request_muxed_region(ioreg, 2, DRVNAME))
        return -EBUSY;

    outb(0x87, ioreg);
    outb(0x87, ioreg);

    return 0;
}

    static inline void
superio_select(int ioreg, int ld)
{
    outb(SIO_REG_LDSEL, ioreg);
    outb(ld, ioreg + 1);
}

    static inline void
superio_exit(int ioreg)
{
    outb(0xaa, ioreg);
    outb(0x02, ioreg);
    outb(0x02, ioreg + 1);
    release_region(ioreg, 2);
}

/*
 * ISA constants
 */

#define IOREGION_ALIGNMENT	(~7)
#define IOREGION_OFFSET		5
#define IOREGION_LENGTH		2
#define ADDR_REG_OFFSET		0
#define DATA_REG_OFFSET		1

#define NCT6775_REG_BANK	0x4E
#define NCT6775_REG_CONFIG	0x40

static int superio_proc(struct sio_smbus_priv *priv)
{
    int ret = 0;
    int sioaddr = SIO_ADDR;
    int addr;
    enum kinds kind;
    int enable_val;
    u16 val;

    ret = superio_enter(sioaddr);
    if (ret)
        return ret;

    superio_select(sioaddr, NCT6775_LD_HWM);
    val = (superio_inb(sioaddr, 0x62) << 8) | superio_inb(sioaddr, 0x63 );

    priv->bs_addr = val & BS_MASK;
    enable_val = superio_inb(sioaddr, SIO_REG_ENABLE);
    if (!enable_val){
        superio_outb(SIO_ADDR, 0x30, enable_val | 0x01);
    }

    val = (superio_inb(sioaddr, SIO_REG_DEVID) << 8) | superio_inb(sioaddr, SIO_REG_DEVID + 1);
    switch (val & SIO_ID_MASK) {
        case SIO_NCT6106_ID:
            kind = nct6106;
            break;
        case SIO_NCT6775_ID:
            kind = nct6775;
            break;
        case SIO_NCT6776_ID:
            kind = nct6776;
            break;
        case SIO_NCT6779_ID:
            kind = nct6779;
            break;
        case SIO_NCT6791_ID:
            kind = nct6791;
            break;
        case SIO_NCT6792_ID:
            kind = nct6792;
            break;
        case SIO_NCT6793_ID:
            kind = nct6793;
            break;
        case SIO_NCT6795_ID:
            kind = nct6795;
            break;
        case SIO_NCT6796_ID:
            kind = nct6796;
            break;
        case SIO_NCT6797_ID:
            kind = nct6797;
            break;
        case SIO_NCT6798_ID:
            kind = nct6798;
            break;
        default:
            if (val != 0xffff)
                pr_debug("unsupported chip ID: 0x%04x\n", val);
            superio_exit(sioaddr);
            return -ENODEV;
    }

	superio_outb(SIO_ADDR, 0x1A, 0x0);
	superio_outb(SIO_ADDR, 0x1B, 0x0);
	superio_exit(SIO_ADDR);

	usleep_range(100, 500);
	outb_p(0x0, priv->bs_addr + SM_CONTROL);
    usleep_range(100, 500);
	outb_p(0x40, priv->bs_addr + SM_CONTROL);
	usleep_range(100, 500);
	outb_p(0x0, priv->bs_addr + SM_CONTROL);
    usleep_range(100, 500);
	outb_p(smbus_clock, priv->bs_addr + SM_FERQ);

    /* We have a known chip, find the HWM I/O address */
    superio_select(sioaddr, NCT6775_LD_HWM);
    val = (superio_inb(sioaddr, SIO_REG_ADDR) << 8)
        | superio_inb(sioaddr, SIO_REG_ADDR + 1);
    addr = val & IOREGION_ALIGNMENT;
    if (addr == 0) {
        pr_err("Refusing to enable a Super-I/O device with a base I/O port 0\n");
        superio_exit(sioaddr);
        return -ENODEV;
    }

    superio_exit(sioaddr);
    pr_info("Found %s or compatible chip at %#x:%#x SMBus CLK 0x%x\n",
            nct6775_sio_names[kind], sioaddr, addr, smbus_clock);

    return ret;
}

static s32 sio_smbus_access(struct i2c_adapter *adapter, u16 addr, unsigned short flags, char read_write, u8 command, int size, union i2c_smbus_data *data)
{
    struct sio_smbus_priv *priv = i2c_get_adapdata(adapter);
    int ret = 0;
    int i;
    int len;
    u8 reg = 0, reg1 = 0;
	int start_tx=1;
    mutex_lock(&priv->acpi_lock);

    outb_p(0x0, priv->bs_addr + SM_CONTROL);
	msleep(1);
    switch (size) {
        case I2C_SMBUS_BYTE:
            //Chip not support the command I2C_SMBUS_BYTE.
			outb_p(((addr & 0x7f) << 1) | (read_write & 0x01), priv->bs_addr + SM_ADDRESS);
            if (read_write == I2C_SMBUS_WRITE){
                outb_p(command, priv->bs_addr + SM_INDEX);
            }
            msleep(1);
            outb_p(0x80, priv->bs_addr + SM_CONTROL);
            break;
        case I2C_SMBUS_BYTE_DATA:
            reg = inb(priv->bs_addr + SM_DATA);
			outb_p(((addr & 0x7f) << 1) | (read_write & 0x01), priv->bs_addr + SM_ADDRESS);
            outb_p(command, priv->bs_addr + SM_INDEX);
            if (read_write == I2C_SMBUS_WRITE){
                outb_p(0x8, priv->bs_addr + SM_COMMAND);
                outb_p(data->byte, priv->bs_addr + SM_DATA);
            }else{
                outb_p(0x0, priv->bs_addr + SM_COMMAND);
            }
            msleep(1);
            outb_p(0x80, priv->bs_addr + SM_CONTROL);
            break;
        case I2C_SMBUS_WORD_DATA:
		    outb_p(((addr & 0x7f) << 1) | (read_write & 0x01), priv->bs_addr + SM_ADDRESS);
            outb_p(command, priv->bs_addr + SM_INDEX);
            if (read_write == I2C_SMBUS_WRITE){
                outb_p(0x9, priv->bs_addr + SM_COMMAND);
                outb_p(data->word & 0xff, priv->bs_addr + SM_DATA);
                outb_p((data->word & 0xff00) >> 8, priv->bs_addr + SM_DATA);
            }else{
				outb_p(0x3, priv->bs_addr + SM_COMMAND);
            }
            msleep(1);
            outb_p(0x80, priv->bs_addr + SM_CONTROL);
            break;
        case I2C_SMBUS_BLOCK_DATA:
			outb_p(((addr & 0x7f) << 1) | (read_write & 0x01), priv->bs_addr + SM_ADDRESS);
            outb_p(command, priv->bs_addr + SM_INDEX);
            if (read_write == I2C_SMBUS_WRITE)
            {
				if (data->block[0] > I2C_SMBUS_BLOCK_MAX-1)
                {
					ret=-EMSGSIZE;
                    goto out;
		        }
		        else if (data->block[0] == 0)
		        {
		            ret=-EBADMSG;
                    goto out;
		        }

				outb_p(0x0A, priv->bs_addr + SM_COMMAND);
				outb_p( data->block[0], priv->bs_addr + SM_WRITE_SIZE);
				len=0;
				for (i=0;i<1000;i++)
				{
					reg = inb(priv->bs_addr + SMCTL3);
					if (reg & 0x02)
					{
						if (((reg & 0x04) == 0) && (start_tx == 1))
						{
							outb_p(0x80, priv->bs_addr + SM_CONTROL);
							usleep_range(10, 50);
							start_tx = 0;
						}
						else
							usleep_range(10, 50);
					}
					else
                    {
						outb_p(data->block[len+1], priv->bs_addr + SM_DATA);
						len++;
					}
					if (len == data->block[0])
                    {
                        if (start_tx == 1)
                        {
                            outb_p(0x80, priv->bs_addr + SM_CONTROL);
                            usleep_range(10, 50);
                        }
                        break;
                    }
				}
            }
            else
            {
                outb_p(0x02, priv->bs_addr + SM_COMMAND);
				msleep(1);
            outb_p(0x80, priv->bs_addr + SM_CONTROL);
			}
            break;
        default:
            printk(KERN_ALERT "Unsupported transaction %d\n",size);
            ret = -EOPNOTSUPP;
            goto out;
    }

	if (read_write == I2C_SMBUS_WRITE)
	{

		for (i=0;i<1000;i++)
		{
			reg = inb(priv->bs_addr + SMCTL3);
			if ((reg & 0x04) == 0)
				break;

			usleep_range(100, 500);
		}

		if (reg & 0x04)
		{
			printk(KERN_ALERT "operation can NOT finish\n");
			ret=-ETIMEDOUT;
			goto out;
		}
		goto out;
	}


    switch (size) {
		case I2C_SMBUS_BYTE:
		case I2C_SMBUS_BYTE_DATA:
			for (i=0;i<1000;i++)
			{
				reg1 = inb_p(priv->bs_addr + SMCTL3);
				if (!(reg1 & 0x01))
				{
					data->byte = inb(priv->bs_addr + SM_DATA);
					break;
				}
				else
					usleep_range(10, 50);
			}
			break;
		case I2C_SMBUS_WORD_DATA:
			data->word = 0;
			len=0;
			for (i=0;i<1000;i++)
			{
				reg1 = inb_p(priv->bs_addr + SMCTL3);
				if (!(reg1 & 0x01))
				{
					data->word = data->word  | (inb_p(priv->bs_addr + SM_DATA) << 8);
					len++;
					if (len == 2)
						break;
				}
				else
					usleep_range(10, 50);
			}
			break;

		case I2C_SMBUS_BLOCK_DATA:
			len = 0;
			data->block[0] = 0;

			for (i=0;i<1000;i++)
			{
				reg1 = inb_p(priv->bs_addr + SMCTL3);
				if (!(reg1 & 0x01))
				{
					data->block[0] = inb(priv->bs_addr + SM_DATA);
					//printk(KERN_ALERT "%s %d should get len=%d\n", __func__,__LINE__,data->block[0]);
					break;
				}
				msleep(2);
			}

			if (data->block[0] == 0)
			{
				printk(KERN_ALERT "%s %d Get data len=0\n", __func__,__LINE__);
				goto out;
			}

			for (i=0;i<1000;i++)
			{
				reg1 = inb_p(priv->bs_addr + SMCTL3);
				if (!(reg1 & 0x01))
				{
					data->block[len+1] = inb(priv->bs_addr + SM_DATA);
					len++;
					if ((len > I2C_SMBUS_BLOCK_MAX-1) || (len == data->block[0]))
						break;
				}
				else
				{
					usleep_range(10, 50);
				}
			}
			if (data->block[0] != len)
			{
				printk(KERN_ALERT "!!!%s %d get len=%d but actual get %d\n", __func__,__LINE__,
						 data->block[0],len);
				data->block[0] = len;
			}
			break;
    }
out:
	//SMBUS software reset
    outb_p(NCT6775_REG_CONFIG, priv->bs_addr + SM_CONTROL);
    mutex_unlock(&priv->acpi_lock);
    return ret;
}

static u32 sio_smbus_func(struct i2c_adapter *adapter)
{
	return (I2C_FUNC_SMBUS_BYTE | I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA |I2C_FUNC_SMBUS_BLOCK_DATA | I2C_FUNC_SMBUS_BLOCK_PROC_CALL);
}

static const struct i2c_algorithm smbus_algorithm = {
    .smbus_xfer	= sio_smbus_access,
    .functionality	= sio_smbus_func,
};

static int sio_smbus_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct sio_smbus_priv *priv = NULL;

    priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    strlcpy(priv->adapter.name, DRVNAME, sizeof(priv->adapter.name));

    priv->adapter.owner = THIS_MODULE;
    priv->adapter.algo = &smbus_algorithm;
    priv->adapter.algo_data = priv;
    priv->adapter.dev.parent = &pdev->dev;
    i2c_set_adapdata(&priv->adapter, priv);
    priv->dev = &pdev->dev;

    platform_set_drvdata(pdev, priv);
    mutex_init(&priv->acpi_lock);
    ret = i2c_add_adapter(&priv->adapter);
    if (ret) {
        printk(KERN_ALERT "failed to add bus to i2c core \n");
        return ret;
    }

    ret = superio_proc(priv);
    printk("ret = %d\n",ret);
    return ret;
}

static int sio_smbus_remove(struct platform_device *pdev)
{
    struct sio_smbus_priv *priv = platform_get_drvdata(pdev);
    i2c_del_adapter(&priv->adapter);
    return 0;
}

static void sio_smbus_release(struct device *dev)
{
    //Nothing to do now, take care iomem resources
    return;
}

static struct platform_driver sio_smbus_driver = {
    .driver		= {
        .name	= DRVNAME,
    },
    .probe		= sio_smbus_probe,
    .remove		= sio_smbus_remove,
};

static struct platform_device sio_smbus_device = {
    .name = DRVNAME,
    .id = -1,
    .dev = {
        .release = sio_smbus_release,
    },
};

static int sio_smbus_init(void)
{
    int ret = 0;
    ret = platform_device_register(&sio_smbus_device);
    if (ret)
        goto exit_error;

    ret = platform_driver_register(&sio_smbus_driver);
    if (ret)
        goto exit_device_unregister;

    printk(KERN_ALERT "SuperIO SMBus driver init \n");
    return 0;

exit_device_unregister:
    platform_device_unregister(&sio_smbus_device);
exit_error:
    return ret;
}

void sio_smbus_exit(void){
    platform_device_unregister(&sio_smbus_device);
    platform_driver_unregister(&sio_smbus_driver);
    printk(KERN_ALERT "SuperIO SMBus driver exit \n");
}

MODULE_AUTHOR("wangzhui <wangzhui@asterfusion.com>");
MODULE_DESCRIPTION("SuperIO SMBus driver");
MODULE_LICENSE("GPL") ;

module_init(sio_smbus_init);
module_exit(sio_smbus_exit);
