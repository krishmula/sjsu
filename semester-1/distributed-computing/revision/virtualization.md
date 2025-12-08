### Virtualization

- method hiding physical characteristics of computer. user sees abstract platform.
- s/w that controls virtz - hypervisor.
- vm: completely isolated guest os installation within host os.
- users isolated from each other.
- vm users have some access to underlying hardware, thru hypervisor.

## Full and Platform Virtualization

- full virtz creates a vm abstraction that's identical to a physical machine.
- guest os unaware that they are being run in a vm
- vmm exports complete instruction set, i/o ops, mem access, etc.
- so, complete simulation of underlying hardware

- platform virtz offers limited simulation of underlying hardware.
- doesn't provide hardware abstraction.
- can impose restrictions on guest os.

- full virtz better, but performance cost, and hardware emulation overhead, and context switching.

## Virtz Types

- Native VM's are where the VMM sits on top of the h/w directly.
- sit directly on top of h/w like CPU, & mem.
- makes hypervisor first layer over h/w, providing direct access to physical resources without an intermediate OS.
-

- Hosted VM's are where the VMM sits on top of a host OS.
- hypervisor the second layer over h/w, and host os.
- must pass h/w reqs through Virtualization layer, and to the host os.
- creates a s/w abstraction, and incr latency.

- Hybrid VM's contain both elements, but less common.
- Xen might be an example.
- support for both paravirtualization, and full hardware Virtualization.

## VMM, VMs, Guest OS

- Virtual Machine Layer: s/w that runs in layer b/w hypervisor or host OS, and the VM's.
- on full virtz, vml is the middleware b/w underlying h/w, and vm in the system.

## Hardware Abstraction Layer

- virtz performed right on top of h/w.
- generates virtual h/w envs for vms.
- manages underlying h/w thru virtz.
- ex: VMWare, Virtual PC, Denali, Xen.
- pros: higher perf., good app isolation
- cons: very expensive, and complex to implement

## Operating Systems Layer

- abstraction layer b/w host OS, and user.
- creates isolated containers on physical server.
- ex: linux containers, docker, etc.
- pros: minimal startup/shutdown cost, low resource req, high scalability
- cons: all vms at os level must have same kind of guest os, poor app isolation
- see fig 6.3 on slides, page 9.

## Hypervisor Types

- Type 1: bare metal Hypervisor. all guest os are a layer above. hypervisor is first layer above h/w.
- Type 2: hosted hypervisor, run over host os. hypervisor is second layer above h/w. ex: FreeBSD. os is usually unaware of the abstraction.

## Xen

- Hypervisor provides hypercalls for the guest OSes, and its apps.
- hypervisor can either assume a micro-kernel architecture (MS Hyper-V), or monolithic architecture (VMWare ESX)

- There's a special domain called Domain 0 in Xen, for Control and I/O.
- Theres several guest domains for user apps.

- Many guest os can run on top of hypervisor.
- the guest os, which has the control ability, is called **Domain 0**.
- others are called **Domain U**.

- Domain 0 is the privileged guest os of XEN.
- it is the first one loaded when XEN boots.
- Domain 0 accesses h/w directly, and managed devices.
- responsibility of Domain 0 is to allocate and map h/w resources for the guest domain (Domain U's).

- Xen is a micro-kernel hypervisor.
- so, it separates policy from mechanism
- Xen hypervisor implements all the mechanism, and Domain 0 handles the policy.
- Xen has no device drivers.
- just provides mechansim for guest os to direclty access physical devices.

## Virtual Networking

- every VM must have network connectivity with other VMs, and to internet.
- a public interface handles comm outside the set of instances.
- private interface used for inter-instance comm.
- a prob is that sys admins/super users may misuse access to the VMs network interface, and may cause interference.

- full virtz hypervisors can have networking capabilities.
- network interfaces that guest os sees might be physical, virtual, or both:
  1. Network Bridging: guest os given direct access to hosts NIC independent of host OS.
  2. Network Address Translation (NAT): guest os given virtual NIC that's connected to simulated NAT inside hypervisor. all outbound traffic sent through virtual NIC to host os, which forwards to physical NIC on the host system.
  3. Host only Networking: guest os given virtual NIC, but doesn't comm with physical NIC directly. guest os comm with each other, and then, potentially with the physical NIC.

## IO Virtualization

- two types: emulation, direct i/o assignment.
- emulation scalable, but perf costs.
- direct i/o perf, but not scalable.
- modern servers run about 50-100 vms, but only have 5-10 i/o devices.

- solution: SR-IOV

## SR-IOV

- allows PCIe devices to present itself as multiple virtual interfaces
- shares single PCIe device, as if there were multiple physical copies.
- so, allows network traffic to bypass hypervisors software switch layer.

- introduces Physical Functions (PF), and Virtual Functions (VF)
- PF: fully featured PCIe funcs. have full config resources. so, possible to configure, and control the device via PF. they're discovered, managed, and manipulated like any other PCIe device.
- PF associated with hypervisors parent parititon or managing os.
- a device usually has a single PF.
- responsible for allocationg and configuring VFs.
- all VFs config should be done via PF.
- so, OS/hypervisor support is needed for SR-IOV, as it needs to properly detect and initialize PF and VFs.

- VF: lightweight funcs, lack config resources. can't be configured, as that would change the underlying config of the PF
- each VF associated with a PF, and shares one or more physical resources of the device, with PF, and other VFs.
- VFs directly assigned to individual VMs.
-

- SRIOV specs say each device can have up to 256 VFs.

- quad port SRIOV NIC's are an exception, as in each port has a PF, which comes out to 1024 VFs.

- practically, 64 is the limit.
